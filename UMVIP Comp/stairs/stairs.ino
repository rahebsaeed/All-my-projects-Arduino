// [STAIR CLIMBING IMPLEMENTATION] ====================================
void climbStairs() {
  Serial.println("Initiating stair climb...");
  
  for(int stair = 0; stair < STAIR_STEPS; stair++) {
    Serial.print("Climbing step ");
    Serial.println(stair+1);
    
    // PHASE 1: Front limbs reach and lift
    for(int angle = 0; angle <= STAIR_HEIGHT; angle += 5) {
      frontLeftKnee.write(FLK_initial - angle);
      frontRightKnee.write(FRK_initial + angle); // Opposite direction for right
      delay(stepDelay/2);
    }
    
    // PHASE 2: Front limbs extend forward
    for(int pos = 0; pos <= STAIR_DEPTH/2; pos += 5) {
      frontLeftHip.write(FLH_initial + pos);
      frontRightHip.write(FRH_initial - pos);
      delay(stepDelay/2);
    }
    
    // PHASE 3: Rear limbs push body up
    for(int angle = 0; angle <= STAIR_HEIGHT; angle += 5) {
      rearLeftKnee.write(RLK_initial - angle);
      rearRightKnee.write(RRK_initial + angle);
      delay(stepDelay/2);
    }
    
    // PHASE 4: Rear limbs step forward
    for(int pos = 0; pos <= STAIR_DEPTH; pos += 5) {
      rearLeftHip.write(RLH_initial + pos);
      rearRightHip.write(RRH_initial - pos);
      delay(stepDelay/2);
    }
    
    // PHASE 5: Stabilize posture
    resetToInitialPositions();
    delay(500);
  }