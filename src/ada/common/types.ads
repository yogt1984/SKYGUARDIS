package Common.Types is

   type Angle_Rad is new Float range -3.14159 .. 3.14159;
   type Range_M is new Float range 0.0 .. 50_000.0;
   type Velocity_Ms is new Float range 0.0 .. 2_000.0;
   type Time_S is new Float range 0.0 .. 60.0;

end Common.Types;

