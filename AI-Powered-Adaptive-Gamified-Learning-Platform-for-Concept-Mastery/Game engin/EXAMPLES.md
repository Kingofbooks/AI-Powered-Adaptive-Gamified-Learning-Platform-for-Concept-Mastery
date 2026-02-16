# Example Scenarios & Presets

## Pre-built Physics Demonstrations

This document contains ready-to-use simulation scenarios for teachers. Simply type or copy-paste the commands!

---

## 🎯 Basic Demonstrations

### 1. Simple Gravity Demo
**Concept:** All objects fall at the same rate

**Setup:**
```
create ground at bottom
create red ball at 200 300
create blue ball at 600 300
```

**Teaching Points:**
- Both balls fall at the same speed
- Mass doesn't affect fall rate (Galileo's discovery!)
- Press P to pause and discuss

---

### 2. Bouncing Ball Energy
**Concept:** Energy is lost in each bounce

**Setup:**
```
create ground at bottom
create ball at top
```

**Teaching Points:**
- Ball bounces lower each time
- Energy converts to heat and sound
- Eventually stops (no perpetual motion!)

---

### 3. Two-Ball Collision
**Concept:** Momentum conservation

**Setup:**
```
create ground at bottom
create red ball at 200 100
create blue ball at 800 100
```

**Instructions:**
1. Let balls fall and collide
2. Pause when they hit: Press P
3. Type: `explain collision`
4. Discuss energy transfer

---

### 4. The Ramp Experiment
**Concept:** Potential → Kinetic energy

**Setup:**
```
create ground at bottom
create ramp at 200 300
create ball at 150 250
```

**Teaching Points:**
- Ball rolls down slope
- Speed increases as height decreases
- Gravity pulls it down the ramp

---

### 5. Stacked Blocks
**Concept:** Stability and balance

**Setup:**
```
create ground at bottom
create block at 400 450
create block at 400 390
create block at 400 330
```

**Instructions:**
- Click top block and drag slightly
- Watch tower collapse!
- Discuss center of gravity

---

## 🔬 Advanced Demonstrations

### 6. Newton's Cradle (Simplified)
**Concept:** Elastic collisions

**Setup:**
```
create ground at bottom
create ball at 200 400
create ball at 300 400
create ball at 400 400
create ball at 500 400
create ball at 600 400
```

**Instructions:**
1. Drag leftmost ball up and to the left
2. Release
3. Watch energy transfer through balls!

---

### 7. Projectile Motion
**Concept:** Parabolic trajectory

**Setup:**
```
create ground at bottom
create ramp at 100 300
create ball at 80 250
```

**Instructions:**
1. Ball rolls down ramp
2. Launches into air
3. Follows curved path (parabola)
4. Select ball and type: `explain velocity`

---

### 8. Different Materials
**Concept:** Restitution (bounciness)

**Setup:**
```
create ground at bottom
create ball at 300 200
create block at 600 200
```

**Teaching Points:**
- Ball is very bouncy (restitution = 0.75)
- Block is less bouncy (restitution = 0.4)
- Compare bounce heights!

---

### 9. Marble Run
**Concept:** Multiple energy conversions

**Setup:**
```
create ground at bottom
create ramp at 100 200
create ground at 350 300
create ramp at 400 350
create ball at 50 150
```

**Instructions:**
- Ball rolls down first ramp
- Lands on platform
- Rolls down second ramp
- Complex motion path!

---

### 10. Collision Chain Reaction
**Concept:** Force transmission

**Setup:**
```
create ground at bottom
create block at 300 450
create block at 400 450
create block at 500 450
create block at 600 450
create ball at 100 200
```

**Instructions:**
1. Ball falls from height
2. Hits first block
3. Watch dominoes fall!

---

## 📚 Classroom Lesson Scenarios

### Lesson: Introduction to Gravity

**Part 1: The Drop Test (5 min)**
```
create ball at top
```
- Ask: "What will happen?"
- Run simulation
- Pause halfway: Press P
- Type: `explain gravity`

**Part 2: Same Fall Rate (7 min)**
```
clear
create ground at bottom
create red ball at 300 100
create blue ball at 700 100
```
- Ask: "Which falls faster?"
- Students predict
- Run and observe: Same rate!
- Historical context: Galileo's experiment

**Part 3: Different Heights (8 min)**
```
clear
create ground at bottom
create ball at 300 100
create ball at 700 300
```
- Ball from higher position
- Takes longer but same acceleration
- Discuss: distance = ½at²

---

### Lesson: Energy and Collisions

**Part 1: The Bounce (5 min)**
```
create ground at bottom
create ball at top
```
- Count bounces
- Measure heights (visually)
- Where does energy go?

**Part 2: Head-On Collision (8 min)**
```
clear
create ground at bottom
create red ball at 200 100
create blue ball at 800 100
```
- Pause at collision: P
- Type: `explain collision`
- Type: `explain energy` (select a ball first)

**Part 3: The Ramp (7 min)**
```
clear
create ground at bottom
create ramp at 200 350
create ball at 150 300
```
- Potential energy at top
- Kinetic energy at bottom
- Conservation of energy!

---

### Lesson: Newton's Laws

**Part 1: First Law (Inertia)**
```
create block at 400 300
```
- Block stays still (at rest)
- Drag it: Stays where placed (uniform motion)
- Objects don't change motion without force

**Part 2: Second Law (F=ma)**
```
clear
create ground at bottom
create ball at 400 200
```
- Type: `explain energy` (with ball selected)
- Heavier balls harder to accelerate
- Force = mass × acceleration

**Part 3: Third Law (Action-Reaction)**
```
clear
create ground at bottom
create red ball at 300 300
create blue ball at 700 300
```
- Balls collide
- Equal and opposite forces
- Both change direction

---

## 🎨 Creative Challenges for Students

### Challenge 1: The Longest Bounce
**Goal:** Make ball bounce as many times as possible

**Constraints:**
- Must use exactly 1 ball
- Must use exactly 1 ground
- Can adjust positions

**Hint:** Drop from very high!

---

### Challenge 2: The Marble Maze
**Goal:** Get ball from top-left to bottom-right

**Constraints:**
- Use ramps and platforms
- Ball must touch at least 3 surfaces
- Must change direction at least once

---

### Challenge 3: The Tower Toppler
**Goal:** Knock down a tower with one ball

**Setup:**
```
create ground at bottom
create block at 600 450
create block at 600 390
create block at 600 330
```

**Challenge:** 
- Add ONE ball
- Position it to knock down all blocks
- Fewest tries wins!

---

### Challenge 4: The Energy Converter
**Goal:** Convert potential energy to maximum kinetic

**Task:**
- Create highest possible drop
- Measure final velocity (select ball, explain velocity)
- Highest speed wins!

---

### Challenge 5: The Perfect Ramp
**Goal:** Launch ball to land on target

**Setup:**
```
create ground at 900 500
create ground at bottom
```

**Challenge:**
- Add ramp to launch ball
- Ball must land on right platform
- Adjust ramp angle by repositioning

---

## 🧪 Physics Experiments

### Experiment 1: Measuring Gravity

**Hypothesis:** Gravity accelerates all objects equally

**Procedure:**
1. Create ground at bottom
2. Create 3 balls at different heights
3. Time how long each takes to hit ground
4. Record and compare

**Analysis:**
- All should have same acceleration
- Time depends on height: t = √(2h/g)

---

### Experiment 2: Coefficient of Restitution

**Hypothesis:** Different materials bounce differently

**Procedure:**
```
create ground at bottom
create ball at 400 100
```

1. Observe bounce height
2. Estimate restitution coefficient
3. Clear and try with block
4. Compare results

**Expected:**
- Ball: ~75% bounce back
- Block: ~40% bounce back

---

### Experiment 3: Collision Momentum

**Hypothesis:** Momentum is conserved in collisions

**Setup:**
```
create ground at bottom
create red ball at 200 200
create blue ball at 800 200
```

**Procedure:**
1. Note initial velocities (both at rest)
2. Let balls fall and collide
3. Pause at collision
4. Type: `explain velocity` for each ball
5. Calculate total momentum before/after

---

### Experiment 4: Inclined Plane Mechanics

**Hypothesis:** Balls accelerate faster on steeper ramps

**Test:**
```
create ground at bottom
create ramp at 200 300
create ball at 150 250
```

1. Time how long ball takes to reach bottom
2. Move ramp to change angle
3. Compare times
4. Steeper = faster

---

## 💡 Teaching Tips

### Making Demonstrations Engaging

**Before Running:**
- Ask for predictions
- Have students vote
- Write predictions on board

**During Simulation:**
- Narrate what's happening
- Point out key moments
- Pause at critical points

**After Simulation:**
- Compare to predictions
- Discuss surprises
- Answer questions

### Using the Pause Feature

**Strategic Pausing:**
- Pause just before collision
- Pause at highest bounce point
- Pause when something unexpected happens

**While Paused:**
- Ask questions
- Draw on board
- Type `explain` commands

### Encouraging Participation

**Let Students:**
- Suggest object positions
- Choose colors
- Type commands
- Drag objects
- Create challenges

---

## 📊 Assessment Ideas

### Quick Checks
1. Predict outcome before simulation
2. Explain why it happened
3. Sketch the motion path
4. Calculate velocities

### Project Ideas
1. Design a marble run
2. Create a domino effect
3. Build a stable structure
4. Demonstrate a physics concept

### Lab Reports
- Hypothesis
- Setup (list commands)
- Observations
- Conclusion

---

## 🎯 Common Misconceptions to Address

### "Heavier objects fall faster"
**Demo:** Create two balls at same height
**Truth:** All objects fall at same rate (in vacuum/no air resistance)

### "Objects need force to keep moving"
**Demo:** Drag block across screen, release
**Truth:** Objects maintain motion without force (Newton's 1st Law)

### "Bouncing ball will eventually bounce forever"
**Demo:** Single ball on ground
**Truth:** Energy lost to heat/sound, bounces stop

### "Ramps make things fall slower"
**Demo:** Ball on ramp vs free fall
**Truth:** Component of gravity along ramp is less

---

## 🔄 Daily Warm-Up Challenges

### Monday: Mystery Setup
Teacher creates scenario, students predict outcome

### Tuesday: Fastest Setup
Students race to recreate yesterday's demo

### Wednesday: What's Wrong?
Teacher creates impossible scenario, students identify error

### Thursday: Creative Challenge
Students design most interesting bounce pattern

### Friday: Review Quiz
Students create demo showing concept learned this week

---

**Use these scenarios to make physics come alive! Copy commands exactly as shown, or modify for your needs.**
