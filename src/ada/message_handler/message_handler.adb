with GNAT.Sockets;
with Interfaces;
with Ada.Text_IO;
with System;
with Ada.Unchecked_Conversion;

package body Message_Handler is

   use GNAT.Sockets;
   use Interfaces;

   -- Helper function to convert network byte order to host byte order for 32-bit
   function Ntohl (Value : Unsigned_32) return Unsigned_32 is
      -- Simplified: assume little-endian host, network is big-endian
      -- In production, would check endianness
      Result : Unsigned_32;
   begin
      Result := (Value and 16#000000FF#) * 16#1000000# +
                (Value and 16#0000FF00#) * 16#0000100# +
                (Value and 16#00FF0000#) / 16#0000100# +
                (Value and 16#FF000000#) / 16#1000000#;
      return Result;
   end Ntohl;

   -- Helper to extract IEEE 754 double from bytes
   function Bytes_To_Double (Bytes : String) return Float is
      -- Extract IEEE 754 double (simplified - assumes same endianness for now)
      -- In production, would properly handle network byte order
      type Float_Access is access Float;
      function To_Access is new Ada.Unchecked_Conversion (System.Address, Float_Access);
      Bytes_Addr : constant System.Address := Bytes'Address;
   begin
      -- Direct memory copy (assumes host and network have same endianness)
      -- For production, would convert from network byte order
      return To_Access (Bytes_Addr).all;
   end Bytes_To_Double;

   procedure Initialize (
      Handler : in out Message_Handler_Type;
      Port    : Port_Type
   ) is
      Address : Sock_Addr_Type;
      Success : Boolean := True;
   begin
      if Handler.Initialized then
         return;
      end if;
      
      -- Create socket
      Create_Socket (Handler.Socket, Family_Inet, Socket_Datagram);
      
      -- Setup address
      Address.Addr := Inet_Addr ("127.0.0.1");
      Address.Port := Port_Type'Pos (Port);
      
      -- Bind socket
      Bind_Socket (Handler.Socket, Address);
      
      Handler.Receive_Port := Port;
      Handler.Initialized := True;
      
   exception
      when others =>
         if Handler.Socket >= 0 then
            Close_Socket (Handler.Socket);
            Handler.Socket := -1;
         end if;
         Handler.Initialized := False;
         Success := False;
   end Initialize;

   function Receive_Target_Assignment (
      Handler : in out Message_Handler_Type;
      Message : out Target_Assignment_Message;
      Success : out Boolean
   ) return Boolean is
      Buffer : String (1 .. 43);
      Last   : Natural;
      From   : Sock_Addr_Type;
      Offset : Natural;
   begin
      Success := False;
      
      if not Handler.Initialized then
         return False;
      end if;
      
      -- Try to receive (non-blocking would be better, but simplified for now)
      begin
         Receive_Socket (Handler.Socket, Buffer, Last, From);
         
         if Last < 43 then
            return False;
         end if;
         
         -- Validate message type
         if Character'Pos (Buffer (1)) /= 1 then
            return False;  -- Not TARGET_ASSIGNMENT
         end if;
         
         -- Validate version
         if Character'Pos (Buffer (2)) /= 1 then
            return False;
         end if;
         
         -- Extract checksum (bytes 5-6, network byte order)
         declare
            Checksum_Net : Unsigned_16;
            Checksum_Calc : Unsigned_32 := 0;
         begin
            -- Extract checksum (simplified - proper would use ntohs)
            Checksum_Net := Unsigned_16 (Character'Pos (Buffer (5))) * 256 +
                           Unsigned_16 (Character'Pos (Buffer (6)));
            
            -- Calculate checksum (excluding checksum field)
            for I in 1 .. 4 loop
               Checksum_Calc := Checksum_Calc + Unsigned_32 (Character'Pos (Buffer (I)));
            end loop;
            for I in 7 .. 43 loop
               Checksum_Calc := Checksum_Calc + Unsigned_32 (Character'Pos (Buffer (I)));
            end loop;
            Checksum_Calc := Checksum_Calc and 16#FFFF#;
            
            -- Validate checksum
            if Unsigned_16 (Checksum_Calc) /= Checksum_Net then
               return False;
            end if;
         end;
         
         -- Deserialize payload (starting at offset 7, after header)
         Offset := 7;
         
         -- Extract target ID (4 bytes, network byte order)
         declare
            Target_ID_Net : Unsigned_32;
         begin
            Target_ID_Net := Unsigned_32 (Character'Pos (Buffer (Offset))) * 16#1000000# +
                            Unsigned_32 (Character'Pos (Buffer (Offset + 1))) * 16#0000100# +
                            Unsigned_32 (Character'Pos (Buffer (Offset + 2))) * 16#0000001# +
                            Unsigned_32 (Character'Pos (Buffer (Offset + 3)));
            Message.Target_ID := Target_ID_Net;
         end;
         Offset := Offset + 4;
         
         -- Extract doubles (8 bytes each, IEEE 754)
         Message.Range_M := Bytes_To_Double (Buffer (Offset .. Offset + 7));
         Offset := Offset + 8;
         
         Message.Azimuth_Rad := Bytes_To_Double (Buffer (Offset .. Offset + 7));
         Offset := Offset + 8;
         
         Message.Elevation_Rad := Bytes_To_Double (Buffer (Offset .. Offset + 7));
         Offset := Offset + 8;
         
         Message.Velocity_Ms := Bytes_To_Double (Buffer (Offset .. Offset + 7));
         Offset := Offset + 8;
         
         -- Extract priority (1 byte)
         Message.Priority := Unsigned_8 (Character'Pos (Buffer (Offset)));
         
         Success := True;
         return True;
      exception
         when Socket_Error =>
            return False;
         when others =>
            return False;
      end;
   end Receive_Target_Assignment;

   procedure Send_Engagement_Status (
      Handler : in out Message_Handler_Type;
      Status  : Engagement_Status_Message;
      Success : out Boolean
   ) is
      Buffer : String (1 .. 28);
      Address : Sock_Addr_Type;
      Offset : Natural := 1;
      Checksum : Unsigned_32 := 0;
   begin
      Success := False;
      
      if not Handler.Initialized then
         return;
      end if;
      
      -- Serialize message header
      Buffer (Offset) := Character'Val (2); -- Message type: ENGAGEMENT_STATUS
      Checksum := Checksum + 2;
      Offset := Offset + 1;
      
      Buffer (Offset) := Character'Val (1); -- Version
      Checksum := Checksum + 1;
      Offset := Offset + 1;
      
      -- Length (22 bytes payload, network byte order)
      Buffer (Offset) := Character'Val (0);
      Buffer (Offset + 1) := Character'Val (22);
      Checksum := Checksum + 22;
      Offset := Offset + 2;
      
      -- Placeholder for checksum
      declare
         Checksum_Pos : constant Natural := Offset;
      begin
         Offset := Offset + 2;
         
         -- Serialize payload
         -- Target ID (4 bytes, network byte order)
         Buffer (Offset) := Character'Val (Natural (Status.Target_ID) / 16#1000000# mod 256);
         Buffer (Offset + 1) := Character'Val (Natural (Status.Target_ID) / 16#0000100# mod 256);
         Buffer (Offset + 2) := Character'Val (Natural (Status.Target_ID) / 16#0000001# mod 256);
         Buffer (Offset + 3) := Character'Val (Natural (Status.Target_ID) mod 256);
         Checksum := Checksum + Unsigned_32 (Status.Target_ID) and 16#FF#;
         Offset := Offset + 4;
         
         Buffer (Offset) := Character'Val (Status.State);
         Checksum := Checksum + Unsigned_32 (Status.State);
         Offset := Offset + 1;
         
         Buffer (Offset) := Character'Val (Status.Firing);
         Checksum := Checksum + Unsigned_32 (Status.Firing);
         Offset := Offset + 1;
         
         -- Doubles (serialize IEEE 754)
         declare
            Lead_Angle_Str : String (1 .. 8);
            Time_To_Impact_Str : String (1 .. 8);
            for Lead_Angle_Str'Address use Status.Lead_Angle_Rad'Address;
            for Time_To_Impact_Str'Address use Status.Time_To_Impact_S'Address;
         begin
            -- Direct memory copy (simplified - assumes same endianness)
            Buffer (Offset .. Offset + 7) := Lead_Angle_Str;
            for I in 1 .. 8 loop
               Checksum := Checksum + Unsigned_32 (Character'Pos (Lead_Angle_Str (I)));
            end loop;
            Offset := Offset + 8;
            Buffer (Offset .. Offset + 7) := Time_To_Impact_Str;
            for I in 1 .. 8 loop
               Checksum := Checksum + Unsigned_32 (Character'Pos (Time_To_Impact_Str (I)));
            end loop;
            Offset := Offset + 8;
         end;
         
         -- Write checksum
         Checksum := Checksum and 16#FFFF#;
         Buffer (Checksum_Pos) := Character'Val (Natural (Checksum) / 256);
         Buffer (Checksum_Pos + 1) := Character'Val (Natural (Checksum) mod 256);
      end;
      
      -- Setup send address
      Address.Addr := Inet_Addr ("127.0.0.1");
      Address.Port := Port_Type'Pos (Handler.Send_Port);
      
      -- Send message
      begin
         Send_Socket (Handler.Socket, Buffer, Address);
         Success := True;
      exception
         when others =>
            Success := False;
      end;
   end Send_Engagement_Status;

   procedure Shutdown (Handler : in out Message_Handler_Type) is
   begin
      if Handler.Socket >= 0 then
         Close_Socket (Handler.Socket);
         Handler.Socket := -1;
      end if;
      Handler.Initialized := False;
   end Shutdown;

   function Is_Initialized (Handler : Message_Handler_Type) return Boolean is
   begin
      return Handler.Initialized;
   end Is_Initialized;

end Message_Handler;
