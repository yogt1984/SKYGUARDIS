with GNAT.Sockets;
with Interfaces;
with Ada.Text_IO;
with System;

package body Message_Handler is

   use GNAT.Sockets;
   use Interfaces;

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
      
      -- Set socket to non-blocking (simplified - would use Set_Socket_Option in production)
      
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
   begin
      Success := False;
      
      if not Handler.Initialized then
         return False;
      end if;
      
      -- Try to receive (non-blocking would be better, but simplified for now)
      begin
         Receive_Socket (Handler.Socket, Buffer, Last, From);
         
         if Last < Buffer'Length then
            return False;
         end if;
         
         -- Deserialize message (simplified - proper deserialization would handle endianness)
         declare
            Offset : Natural := 7; -- Skip header (6 bytes) + checksum validation skipped for now
            Target_ID_Net : Unsigned_32;
            Range_M_Bytes : String (1 .. 8);
            Azimuth_Bytes : String (1 .. 8);
            Elevation_Bytes : String (1 .. 8);
            Velocity_Bytes : String (1 .. 8);
         begin
            -- Extract target ID (network byte order)
            Target_ID_Net := Unsigned_32'Value (Buffer (Offset .. Offset + 3));
            Message.Target_ID := Target_ID_Net; -- Simplified - should convert from network byte order
            Offset := Offset + 4;
            
            -- Extract doubles (simplified - proper implementation would handle IEEE 754)
            Range_M_Bytes := Buffer (Offset .. Offset + 7);
            Message.Range_M := Float'Value (Range_M_Bytes); -- Simplified
            Offset := Offset + 8;
            
            Azimuth_Bytes := Buffer (Offset .. Offset + 7);
            Message.Azimuth_Rad := Float'Value (Azimuth_Bytes); -- Simplified
            Offset := Offset + 8;
            
            Elevation_Bytes := Buffer (Offset .. Offset + 7);
            Message.Elevation_Rad := Float'Value (Elevation_Bytes); -- Simplified
            Offset := Offset + 8;
            
            Velocity_Bytes := Buffer (Offset .. Offset + 7);
            Message.Velocity_Ms := Float'Value (Velocity_Bytes); -- Simplified
            Offset := Offset + 8;
            
            Message.Priority := Unsigned_8'Value (Buffer (Offset .. Offset));
            
            Success := True;
            return True;
         end;
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
   begin
      Success := False;
      
      if not Handler.Initialized then
         return;
      end if;
      
      -- Serialize message (simplified)
      Buffer (Offset) := Character'Val (2); -- Message type: ENGAGEMENT_STATUS
      Offset := Offset + 1;
      Buffer (Offset) := Character'Val (1); -- Version
      Offset := Offset + 1;
      -- Length and checksum skipped for simplified version
      Offset := 7;
      
      -- Serialize payload (simplified - proper implementation needed)
      declare
         Target_ID_Str : String := Unsigned_32'Image (Status.Target_ID);
      begin
         -- Simplified serialization - would need proper binary format
         Buffer (Offset .. Offset + 3) := Target_ID_Str;
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

