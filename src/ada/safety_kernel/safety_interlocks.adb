package body Safety_Kernel.Safety_Interlocks is

   function Is_Safe_To_Fire (
      Azimuth_Rad : Angle_Rad;
      Elevation_Rad : Elevation_Rad;
      Range_M : Range_M
   ) return Safety_Status is
   begin
      if not Validate_Angle_Limits (Azimuth_Rad, Elevation_Rad) then
         return Unsafe;
      end if;

      if not Check_No_Fire_Zones (Azimuth_Rad, Elevation_Rad) then
         return Blocked;
      end if;

      if Range_M < 100.0 or Range_M > 10_000.0 then
         return Unsafe;
      end if;

      return Safe;
   end Is_Safe_To_Fire;

   function Check_No_Fire_Zones (
      Azimuth_Rad : Angle_Rad;
      Elevation_Rad : Elevation_Rad
   ) return Boolean is
      -- Simplified: check if angles are within safe firing arcs
      -- In real system, would check against friendly positions, ground, etc.
   begin
      -- Example: no fire below -10 degrees elevation (ground)
      if Elevation_Rad < -0.1745 then
         return False;
      end if;
      return True;
   end Check_No_Fire_Zones;

   function Validate_Angle_Limits (
      Azimuth_Rad : Angle_Rad;
      Elevation_Rad : Elevation_Rad
   ) return Boolean is
   begin
      -- Check azimuth range: -180 to +180 degrees
      if Azimuth_Rad < -3.14159 or Azimuth_Rad > 3.14159 then
         return False;
      end if;

      -- Check elevation range: -90 to +90 degrees
      if Elevation_Rad < -1.5708 or Elevation_Rad > 1.5708 then
         return False;
      end if;

      return True;
   end Validate_Angle_Limits;

end Safety_Kernel.Safety_Interlocks;

