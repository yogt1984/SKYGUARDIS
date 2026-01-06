package Ballistics.Ballistic_Calculator is

   type Range_M is new Float range 0.0 .. 50_000.0;
   type Velocity_Ms is new Float range 0.0 .. 2_000.0;
   type Angle_Rad is new Float range -3.14159 .. 3.14159;
   type Time_S is new Float range 0.0 .. 60.0;

   -- Calculate lead angle for moving target
   function Calculate_Lead_Angle (
      Target_Range_M : Range_M;
      Target_Velocity_Ms : Velocity_Ms;
      Target_Heading_Rad : Angle_Rad;
      Projectile_Velocity_Ms : Velocity_Ms
   ) return Angle_Rad;

   -- Calculate time of flight
   function Calculate_Time_Of_Flight (
      Range_M : Range_M;
      Projectile_Velocity_Ms : Velocity_Ms;
      Elevation_Rad : Angle_Rad
   ) return Time_S;

   -- Simplified ballistic trajectory calculation
   function Calculate_Trajectory (
      Range_M : Range_M;
      Elevation_Rad : Angle_Rad;
      Projectile_Velocity_Ms : Velocity_Ms
   ) return Time_S;

end Ballistics.Ballistic_Calculator;

