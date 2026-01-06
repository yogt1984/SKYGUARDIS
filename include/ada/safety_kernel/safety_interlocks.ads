package Safety_Kernel.Safety_Interlocks is

   type Angle_Rad is new Float range -3.14159 .. 3.14159;
   type Range_M is new Float range 0.0 .. 50_000.0;
   type Elevation_Rad is new Float range -1.5708 .. 1.5708;

   type Safety_Status is (Safe, Unsafe, Blocked);

   -- Check if firing is safe at given azimuth and elevation
   function Is_Safe_To_Fire (
      Azimuth_Rad : Angle_Rad;
      Elevation_Rad : Elevation_Rad;
      Range_M : Range_M
   ) return Safety_Status;

   -- Check no-fire zones (e.g., friendly positions, ground)
   function Check_No_Fire_Zones (
      Azimuth_Rad : Angle_Rad;
      Elevation_Rad : Elevation_Rad
   ) return Boolean;

   -- Validate angle limits
   function Validate_Angle_Limits (
      Azimuth_Rad : Angle_Rad;
      Elevation_Rad : Elevation_Rad
   ) return Boolean;

end Safety_Kernel.Safety_Interlocks;

