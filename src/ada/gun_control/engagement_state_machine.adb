with Ada.Real_Time;
with Safety_Kernel.Safety_Interlocks;

package body Gun_Control.Engagement_State_Machine is
   use Safety_Kernel.Safety_Interlocks;

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
   begin
      -- Update target data if provided
      if Target_Range_M > 0.0 then
         Context.Target_Range_M := Target_Range_M;
         Context.Target_Azimuth_Rad := Target_Azimuth_Rad;
         Context.Target_Elevation_Rad := Target_Elevation_Rad;
      end if;
      case Context.State is
         when Idle =>
            if Command = Start_Engagement then
               -- Pre-acquisition safety check
               declare
                  Safety_Check : Safety_Status;
               begin
                  Safety_Check := Is_Safe_To_Fire (
                     Angle_Rad (Context.Target_Azimuth_Rad),
                     Elevation_Rad (Context.Target_Elevation_Rad),
                     Range_M (Context.Target_Range_M)
                  );
                  if Safety_Check = Safe then
                     Context.State := Acquiring;
                  else
                     -- Safety violation: remain in Idle
                     Context.State := Idle;
                  end if;
               end;
            end if;
         when Acquiring =>
            if Command = Continue_Tracking then
               Context.State := Tracking;
            elsif Command = Abort then
               Context.State := Idle;
            end if;
         when Tracking =>
            if Command = Fire then
               -- Pre-fire safety check
               declare
                  Safety_Check : Safety_Status;
               begin
                  Safety_Check := Is_Safe_To_Fire (
                     Angle_Rad (Context.Target_Azimuth_Rad),
                     Elevation_Rad (Context.Target_Elevation_Rad),
                     Range_M (Context.Target_Range_M)
                  );
                  if Safety_Check = Safe then
                     Context.State := Firing;
                  else
                     -- Safety violation: abort engagement
                     Context.State := Idle;
                  end if;
               end;
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

   procedure Set_Target_Data (
      Context : in out Engagement_Context;
      Target_ID : Natural;
      Range_M : Float;
      Azimuth_Rad : Float;
      Elevation_Rad : Float;
      Velocity_Ms : Float
   ) is
   begin
      Context.Target_Id := Target_ID;
      Context.Target_Range_M := Range_M;
      Context.Target_Azimuth_Rad := Azimuth_Rad;
      Context.Target_Elevation_Rad := Elevation_Rad;
      Context.Target_Velocity_Ms := Velocity_Ms;
   end Set_Target_Data;

   function Get_Target_ID (Context : Engagement_Context) return Natural is
   begin
      return Context.Target_Id;
   end Get_Target_ID;

   function Get_Target_Range (Context : Engagement_Context) return Float is
   begin
      return Context.Target_Range_M;
   end Get_Target_Range;

   function Get_Target_Azimuth (Context : Engagement_Context) return Float is
   begin
      return Context.Target_Azimuth_Rad;
   end Get_Target_Azimuth;

   function Get_Target_Elevation (Context : Engagement_Context) return Float is
   begin
      return Context.Target_Elevation_Rad;
   end Get_Target_Elevation;

   function Get_Target_Velocity (Context : Engagement_Context) return Float is
   begin
      return Context.Target_Velocity_Ms;
   end Get_Target_Velocity;

end Gun_Control.Engagement_State_Machine;

