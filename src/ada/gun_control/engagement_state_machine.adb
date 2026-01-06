with Ada.Real_Time;

package body Gun_Control.Engagement_State_Machine is

   procedure Initialize (Context : out Engagement_Context) is
   begin
      Context.State := Idle;
      Context.Target_Id := 0;
      Context.Last_Update_Time := Ada.Real_Time.Clock;
   end Initialize;

   procedure Process_Command (
      Context : in out Engagement_Context;
      Command : Engagement_Command;
      Target_Range_M : Float;
      Target_Azimuth_Rad : Float;
      Target_Elevation_Rad : Float
   ) is
      pragma Unreferenced (Target_Range_M, Target_Azimuth_Rad, Target_Elevation_Rad);
   begin
      case Context.State is
         when Idle =>
            if Command = Start_Engagement then
               Context.State := Acquiring;
            end if;
         when Acquiring =>
            if Command = Continue_Tracking then
               Context.State := Tracking;
            elsif Command = Abort then
               Context.State := Idle;
            end if;
         when Tracking =>
            if Command = Fire then
               Context.State := Firing;
            elsif Command = Abort then
               Context.State := Idle;
            end if;
         when Firing =>
            Context.State := Verifying;
         when Verifying =>
            Context.State := Complete;
         when Complete =>
            Context.State := Idle;
      end case;
      Context.Last_Update_Time := Ada.Real_Time.Clock;
   end Process_Command;

   function Current_State (Context : Engagement_Context) return Engagement_State is
   begin
      return Context.State;
   end Current_State;

end Gun_Control.Engagement_State_Machine;

