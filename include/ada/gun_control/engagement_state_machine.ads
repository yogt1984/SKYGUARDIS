with Ada.Real_Time;

package Gun_Control.Engagement_State_Machine is

   type Engagement_State is (
      Idle,
      Acquiring,
      Tracking,
      Firing,
      Verifying,
      Complete
   );

   type Engagement_Command is (
      Start_Engagement,
      Continue_Tracking,
      Fire,
      Abort
   );

   type Engagement_Context is private;

   procedure Initialize (Context : out Engagement_Context);

   procedure Process_Command (
      Context : in out Engagement_Context;
      Command : Engagement_Command;
      Target_Range_M : Float;
      Target_Azimuth_Rad : Float;
      Target_Elevation_Rad : Float
   );

   function Current_State (Context : Engagement_Context) return Engagement_State;

private

   type Engagement_Context is record
      State : Engagement_State := Idle;
      Target_Id : Natural := 0;
      Last_Update_Time : Ada.Real_Time.Time;
   end record;

end Gun_Control.Engagement_State_Machine;

