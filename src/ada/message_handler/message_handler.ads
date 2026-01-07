with Interfaces;

package Message_Handler is

   type Port_Type is range 1 .. 65535;
   
   type Message_Handler_Type is limited private;
   
   type Target_Assignment_Message is record
      Target_ID    : Interfaces.Unsigned_32;
      Range_M      : Float;
      Azimuth_Rad  : Float;
      Elevation_Rad : Float;
      Velocity_Ms  : Float;
      Priority     : Interfaces.Unsigned_8;
   end record;
   
   type Engagement_Status_Message is record
      Target_ID        : Interfaces.Unsigned_32;
      State            : Interfaces.Unsigned_8;
      Firing           : Interfaces.Unsigned_8;
      Lead_Angle_Rad   : Float;
      Time_To_Impact_S : Float;
   end record;
   
   procedure Initialize (
      Handler : in out Message_Handler_Type;
      Port    : Port_Type
   );
   
   function Receive_Target_Assignment (
      Handler : in out Message_Handler_Type;
      Message : out Target_Assignment_Message;
      Success : out Boolean
   ) return Boolean;
   
   procedure Send_Engagement_Status (
      Handler : in out Message_Handler_Type;
      Status  : Engagement_Status_Message;
      Success : out Boolean
   );
   
   procedure Shutdown (Handler : in out Message_Handler_Type);
   
   function Is_Initialized (Handler : Message_Handler_Type) return Boolean;

private
   type Message_Handler_Type is limited record
      Socket      : Integer := -1;
      Initialized : Boolean := False;
      Receive_Port : Port_Type := 8888;
      Send_Port     : Port_Type := 8889;
   end record;

end Message_Handler;

