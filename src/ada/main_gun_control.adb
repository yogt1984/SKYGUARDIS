with Gun_Control.Engagement_State_Machine;
with Safety_Kernel.Safety_Interlocks;
with Ada.Text_IO;
with Ada.Real_Time;

procedure Main_Gun_Control is
   use Gun_Control.Engagement_State_Machine;
   use Safety_Kernel.Safety_Interlocks;
   use Ada.Real_Time;
   
   Context : Engagement_Context;
   Cycle : Natural := 0;
   Period : constant Time_Span := Milliseconds (100);
   Next_Time : Time := Clock + Period;
begin
   Ada.Text_IO.Put_Line ("[GUN_CTRL] SKYGUARDIS Gun Control Computer starting...");
   
   Initialize (Context);
   Ada.Text_IO.Put_Line ("[GUN_CTRL] Gun Control Computer initialized");
   
   -- Main real-time control loop
   loop
      -- Periodic control task
      declare
         Current_State : Engagement_State := Current_State (Context);
      begin
         -- Process engagement state machine
         if Current_State = Idle then
            -- Wait for target assignment
            null;
         else
            Ada.Text_IO.Put_Line ("[GUN_CTRL] Cycle" & Natural'Image (Cycle) & 
                                 ": State=" & Engagement_State'Image (Current_State));
         end if;
      end;
      
      Cycle := Cycle + 1;
      if Cycle mod 100 = 0 then
         Ada.Text_IO.Put_Line ("[GUN_CTRL] Running - cycle" & Natural'Image (Cycle));
      end if;
      
      -- Wait until next period
      delay until Next_Time;
      Next_Time := Next_Time + Period;
   end loop;
exception
   when others =>
      Ada.Text_IO.Put_Line ("[GUN_CTRL] Shutdown complete");
end Main_Gun_Control;

