with Gun_Control.Engagement_State_Machine;
with Safety_Kernel.Safety_Interlocks;
with Ada.Text_IO;

procedure Main_Gun_Control is
   use Gun_Control.Engagement_State_Machine;
   use Safety_Kernel.Safety_Interlocks;
   
   Context : Engagement_Context;
begin
   Ada.Text_IO.Put_Line ("SKYGUARDIS Gun Control Computer starting...");
   
   Initialize (Context);
   
   -- Main control loop would go here
   -- Periodic tasks for real-time control
   
   Ada.Text_IO.Put_Line ("Gun Control Computer initialized");
end Main_Gun_Control;

