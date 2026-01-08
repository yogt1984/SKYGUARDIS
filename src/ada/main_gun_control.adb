with Gun_Control.Engagement_State_Machine;
with Safety_Kernel.Safety_Interlocks;
with Ballistics.Ballistic_Calculator;
with Message_Handler;
with Ada.Text_IO;
with Ada.Real_Time;
with Interfaces;
with Ada.Task_Identification;
with System;

procedure Main_Gun_Control is
   use Gun_Control.Engagement_State_Machine;
   use Safety_Kernel.Safety_Interlocks;
   use Ballistics.Ballistic_Calculator;
   use Ada.Real_Time;
   use Interfaces;
   
   Context : Engagement_Context;
   Handler : Message_Handler.Message_Handler_Type;
   Cycle : Natural := 0;
   Period : constant Time_Span := Milliseconds (100);
   Next_Time : Time := Clock + Period;
   Last_State : Engagement_State := Idle;
   Projectile_Velocity : constant Velocity_Ms := 1000.0; -- m/s
   
   -- Graceful shutdown flag
   Shutdown_Requested : Boolean := False;
   pragma Atomic (Shutdown_Requested);
   
   -- Signal handling task with improved error handling
   task Signal_Handler is
      entry Shutdown;
      entry Check_Shutdown (Requested : out Boolean);
   end Signal_Handler;
   
   task body Signal_Handler is
      use Ada.Task_Identification;
      Check_Interval : constant Time_Span := Milliseconds (100);
      Last_Check : Time := Clock;
   begin
      -- Check for shutdown file or signal
      loop
         delay until Last_Check + Check_Interval;
         Last_Check := Clock;
         
         -- Check for stop file (simple mechanism)
         declare
            Stop_File : Ada.Text_IO.File_Type;
         begin
            Ada.Text_IO.Open (Stop_File, Ada.Text_IO.In_File, "/tmp/skyguardis_stop");
            Ada.Text_IO.Close (Stop_File);
            Ada.Text_IO.Put_Line ("[GUN_CTRL] Shutdown requested via stop file");
            Shutdown_Requested := True;
            exit;
         exception
            when Ada.Text_IO.Name_Error =>
               null; -- File doesn't exist, continue
            when others =>
               null;
         end;
         
         -- Exit if shutdown requested via entry call
         select
            accept Shutdown do
               Ada.Text_IO.Put_Line ("[GUN_CTRL] Shutdown requested via entry call");
               Shutdown_Requested := True;
            end Shutdown;
            exit;
         or
            accept Check_Shutdown (Requested : out Boolean) do
               Requested := Shutdown_Requested;
            end Check_Shutdown;
         or
            delay 0.0;
         end select;
      end loop;
   exception
      when others =>
         Ada.Text_IO.Put_Line ("[GUN_CTRL] Signal handler error, requesting shutdown");
         Shutdown_Requested := True;
   end Signal_Handler;
   
begin
   Ada.Text_IO.Put_Line ("[GUN_CTRL] SKYGUARDIS Gun Control Computer starting...");
   Ada.Text_IO.Put_Line ("[GUN_CTRL] Press Ctrl+C or create /tmp/skyguardis_stop to shutdown gracefully");
   
   -- Initialize components
   Initialize (Context);
   
   -- Initialize message handler
   Message_Handler.Initialize (Handler, 8888);
   if not Message_Handler.Is_Initialized (Handler) then
      Ada.Text_IO.Put_Line ("[GUN_CTRL] ERROR: Failed to initialize message handler");
      return;
   end if;
   
   Ada.Text_IO.Put_Line ("[GUN_CTRL] Gun Control Computer initialized");
   
   -- Main real-time control loop
   Main_Loop:
   loop
      -- Check for shutdown request
      if Shutdown_Requested then
         Ada.Text_IO.Put_Line ("[GUN_CTRL] Shutdown requested, cleaning up...");
         exit Main_Loop;
      end if;
      
      -- Check for incoming target assignments
      declare
         Assignment : Message_Handler.Target_Assignment_Message;
         Received   : Boolean;
         Success    : Boolean;
      begin
         if Message_Handler.Receive_Target_Assignment (Handler, Assignment, Success) 
            and then Success
         then
            -- Validate assignment
            if Assignment.Range_M > 0.0 and Assignment.Range_M < 50_000.0 then
               -- Store target data
               Set_Target_Data (
                  Context,
                  Natural (Assignment.Target_ID),
                  Assignment.Range_M,
                  Assignment.Azimuth_Rad,
                  Assignment.Elevation_Rad,
                  Assignment.Velocity_Ms
               );
               
               -- Trigger state machine if in Idle
               if Current_State (Context) = Idle then
                  Process_Command (
                     Context,
                     Start_Engagement,
                     Assignment.Range_M,
                     Assignment.Azimuth_Rad,
                     Assignment.Elevation_Rad
                  );
                  Ada.Text_IO.Put_Line ("[GUN_CTRL] Target assigned: ID=" & 
                                       Natural'Image (Natural (Assignment.Target_ID)) &
                                       " Range=" & Float'Image (Assignment.Range_M) & "m");
               end if;
            end if;
         end if;
      exception
         when others =>
            null; -- Continue on error
      end;
      
      -- Process engagement state machine
      declare
         Current_State_Val : Engagement_State := Current_State (Context);
      begin
         -- State progression logic
         case Current_State_Val is
            when Idle =>
               -- Wait for target assignment (handled above)
               null;
               
            when Acquiring =>
               -- After acquisition period, transition to Tracking
               declare
                  Elapsed : Time_Span := Clock - Context.Last_Update_Time;
               begin
                  if Elapsed > Milliseconds (500) then  -- 500ms acquisition time
                     Process_Command (Context, Continue_Tracking, 0.0, 0.0, 0.0);
                  end if;
               end;
               
            when Tracking =>
               -- Calculate ballistics
               declare
                  Lead_Angle : Angle_Rad;
                  Time_To_Impact : Time_S;
                  Safety_Check : Safety_Status;
               begin
                  -- Continuous safety monitoring
                  Safety_Check := Is_Safe_To_Fire (
                     Angle_Rad (Get_Target_Azimuth (Context)),
                     Elevation_Rad (Get_Target_Elevation (Context)),
                     Range_M (Get_Target_Range (Context))
                  );
                  
                  if Safety_Check /= Safe then
                     -- Safety violation: abort
                     Process_Command (Context, Abort, 0.0, 0.0, 0.0);
                     Ada.Text_IO.Put_Line ("[GUN_CTRL] Safety violation detected - aborting engagement");
                  else
                     -- Calculate lead angle
                     Lead_Angle := Calculate_Lead_Angle (
                        Range_M (Get_Target_Range (Context)),
                        Velocity_Ms (Get_Target_Velocity (Context)),
                        Angle_Rad (0.0),  -- Simplified: assume heading
                        Projectile_Velocity
                     );
                     
                     -- Calculate time of flight
                     Time_To_Impact := Calculate_Time_Of_Flight (
                        Range_M (Get_Target_Range (Context)),
                        Projectile_Velocity,
                        Elevation_Rad (Get_Target_Elevation (Context))
                     );
                     
                     -- Auto-fire after tracking period (simplified)
                     declare
                        Elapsed : Time_Span := Clock - Context.Last_Update_Time;
                     begin
                        if Elapsed > Milliseconds (1000) then  -- 1s tracking time
                           Process_Command (Context, Fire, 0.0, 0.0, 0.0);
                        end if;
                     end;
                  end if;
               end;
               
            when Firing =>
               -- Firing duration
               declare
                  Elapsed : Time_Span := Clock - Context.Last_Update_Time;
               begin
                  if Elapsed > Milliseconds (200) then  -- 200ms firing duration
                     -- Automatic transition to Verifying
                     null;  -- Handled by state machine
                  end if;
               end;
               
            when Verifying =>
               -- Verification period
               declare
                  Elapsed : Time_Span := Clock - Context.Last_Update_Time;
               begin
                  if Elapsed > Milliseconds (500) then  -- 500ms verification
                     -- Automatic transition to Complete
                     null;  -- Handled by state machine
                  end if;
               end;
               
            when Complete =>
               -- Return to Idle
               null;  -- Handled by state machine
         end case;
         
         -- Send status updates on state change
         if Current_State_Val /= Last_State then
            declare
               Status : Message_Handler.Engagement_Status_Message;
               Send_Success : Boolean;
            begin
               Status.Target_ID := Interfaces.Unsigned_32 (Get_Target_ID (Context));
               Status.State := Engagement_State'Pos (Current_State_Val);
               Status.Firing := (if Current_State_Val = Firing then 1 else 0);
               
               -- Calculate lead angle and time-to-impact for status
               if Current_State_Val in Tracking .. Firing then
                  declare
                     Lead_Angle : Angle_Rad := Calculate_Lead_Angle (
                        Range_M (Get_Target_Range (Context)),
                        Velocity_Ms (Get_Target_Velocity (Context)),
                        Angle_Rad (0.0),
                        Projectile_Velocity
                     );
                     Time_To_Impact : Time_S := Calculate_Time_Of_Flight (
                        Range_M (Get_Target_Range (Context)),
                        Projectile_Velocity,
                        Elevation_Rad (Get_Target_Elevation (Context))
                     );
                  begin
                     Status.Lead_Angle_Rad := Float (Lead_Angle);
                     Status.Time_To_Impact_S := Float (Time_To_Impact);
                  end;
               else
                  Status.Lead_Angle_Rad := 0.0;
                  Status.Time_To_Impact_S := 0.0;
               end if;
               
               Message_Handler.Send_Engagement_Status (Handler, Status, Send_Success);
               if Send_Success then
                  Ada.Text_IO.Put_Line ("[GUN_CTRL] Status sent: State=" & 
                                       Engagement_State'Image (Current_State_Val));
               end if;
               
               Last_State := Current_State_Val;
            exception
               when others =>
                  null; -- Continue on error
            end;
         end if;
      exception
         when others =>
            null; -- Continue on error
      end;
      
      Cycle := Cycle + 1;
      if Cycle mod 100 = 0 then
         Ada.Text_IO.Put_Line ("[GUN_CTRL] Running - cycle" & Natural'Image (Cycle) &
                              " State=" & Engagement_State'Image (Current_State (Context)));
      end if;
      
      -- Wait until next period
      delay until Next_Time;
      Next_Time := Next_Time + Period;
   end loop Main_Loop;
   
   -- Cleanup
   Message_Handler.Shutdown (Handler);
   Signal_Handler.Shutdown;
   Ada.Text_IO.Put_Line ("[GUN_CTRL] Shutdown complete");
exception
   when others =>
      Message_Handler.Shutdown (Handler);
      Signal_Handler.Shutdown;
      Ada.Text_IO.Put_Line ("[GUN_CTRL] Shutdown complete (exception)");
end Main_Gun_Control;
