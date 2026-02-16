"""
COMPLETE USAGE EXAMPLES FOR TEACHER AI AGENT API

This file shows practical examples of how to use each endpoint.
Copy and paste these examples into a Python script or Jupyter notebook.
"""

import requests
import json

# ============================================
# CONFIGURATION
# ============================================
API_URL = "http://localhost:8000"

print("=" * 70)
print("TEACHER AI AGENT - COMPLETE USAGE EXAMPLES")
print("=" * 70)
print()

# ============================================
# Example 1: Health Check
# ============================================
print("📋 EXAMPLE 1: Check if API is Running")
print("-" * 70)

try:
    response = requests.get(f"{API_URL}/health")
    result = response.json()
    print(f"✅ API Status: {result['status']}")
    print(f"   Message: {result['message']}")
except:
    print("❌ API is not running. Please start server with: python main.py")
    exit()

print()
print()

# ============================================
# Example 2: Summarize a Concept
# ============================================
print("📚 EXAMPLE 2: Summarize a Teaching Concept")
print("-" * 70)
print("Request: Break down 'Photosynthesis' into simple key points")
print()

concept_data = {
    "concept": "Photosynthesis",
    "level": "intermediate"
}

response = requests.post(f"{API_URL}/summarize", json=concept_data)
summary = response.json()["data"]

print(f"Title: {summary['title']}")
print(f"Level: {summary['level']}")
print(f"\nSummary: {summary['summary'][:150]}...")
print(f"\nKey Points:")
for i, point in enumerate(summary['key_points'], 1):
    print(f"  {i}. {point}")
print(f"\nLearning Objectives:")
for obj in summary['learning_objectives']:
    print(f"  • {obj}")

print()
print()

# ============================================
# Example 3: Generate a Quiz Game
# ============================================
print("🎮 EXAMPLE 3: Generate a Quiz Game")
print("-" * 70)
print("Request: Create a 5-question quiz about 'Photosynthesis'")
print()

quiz_data = {
    "concept": "Photosynthesis",
    "game_type": "quiz",
    "num_questions": 5
}

response = requests.post(f"{API_URL}/generate-quiz", json=quiz_data)
quiz = response.json()["data"]

print(f"Quiz Title: {quiz['title']}")
print(f"Questions: {len(quiz['questions'])}")
print(f"Estimated Duration: {quiz['estimated_duration']} seconds ({quiz['estimated_duration']//60} minutes)")
print(f"\nFirst Question:")
q = quiz['questions'][0]
print(f"  Q: {q['question']}")
print(f"  Options:")
for i, opt in enumerate(q['options']):
    symbol = "✓" if i == q['correct_answer'] else " "
    print(f"    [{symbol}] {i}) {opt}")
print(f"  Explanation: {q['explanation'][:80]}...")
print(f"  Difficulty: {q['difficulty']}")

print("\nScoring Rules:")
for rule, points in quiz['scoring_rules'].items():
    print(f"  • {rule}: {points}")

print()
print()

# ============================================
# Example 4: Generate a Puzzle Game
# ============================================
print("🧩 EXAMPLE 4: Generate a Puzzle Game")
print("-" * 70)
print("Request: Create a logic puzzle about 'The Water Cycle'")
print()

puzzle_data = {
    "concept": "The Water Cycle",
    "level": "intermediate"
}

response = requests.post(f"{API_URL}/generate-puzzle", json=puzzle_data)
puzzle = response.json()["data"]

print(f"Puzzle Title: {puzzle['title']}")
print(f"Type: {puzzle['puzzle_type']}")
print(f"Difficulty: {puzzle['difficulty_level']}")
print(f"\nProblem: {puzzle['content']['problem_statement']}")
print(f"Answer: {puzzle['content']['answer']}")
print(f"\nHints Available:")
for i, hint in enumerate(puzzle['content']['hints'], 1):
    print(f"  Hint {i}: {hint}")
print(f"\nEducational Value:")
print(f"  {puzzle['educational_value']}")

print()
print()

# ============================================
# Example 5: Generate a Speed Challenge
# ============================================
print("⚡ EXAMPLE 5: Generate a Speed Challenge Game")
print("-" * 70)
print("Request: Create 10 timed speed challenges about 'Basic Math'")
print()

speed_data = {
    "concept": "Basic Multiplication",
    "game_type": "speed",
    "num_questions": 10
}

response = requests.post(f"{API_URL}/generate-speed", json=speed_data)
speed = response.json()["data"]

print(f"Challenge Title: {speed['title']}")
print(f"Total Challenges: {len(speed['challenges'])}")
print(f"Total Duration: {speed['total_duration']} seconds ({speed['total_duration']//60} minutes)")
print(f"Difficulty: {speed['difficulty']}")
print(f"\nFirst 3 Challenges:")
for i, challenge in enumerate(speed['challenges'][:3], 1):
    print(f"  {i}. {challenge['prompt']}")
    print(f"     Answer: {challenge['answer']}")
    print(f"     Time Limit: {challenge['time_limit']} seconds")

print("\nScoring System:")
for rule, value in speed['scoring_rules'].items():
    print(f"  • {rule}: {value}")

print()
print()

# ============================================
# Example 6: Get Detailed Game Information
# ============================================
print("📊 EXAMPLE 6: Get Complete Game Details (Format + Instructions)")
print("-" * 70)
print("Request: Get ALL info needed to generate and execute a quiz game")
print()

game_info = {
    "concept": "Newton's Laws of Motion",
    "game_type": "quiz",
    "level": "intermediate",
    "num_questions": 3
}

response = requests.post(f"{API_URL}/game", json=game_info)
game_details = response.json()

print(f"Concept: {game_details['concept']}")
print(f"Game Type: {game_details['game_type']}")
print(f"Level: {game_details['level']}")

print(f"\n1️⃣  LEARNING OBJECTIVES:")
for obj in game_details['objectives']['learning_objectives']:
    print(f"   • {obj}")

print(f"\n2️⃣  GAME FORMAT SPECIFICATION:")
print(f"   Type: {game_details['format']['type']}")
print(f"   Structure includes:")
for key in game_details['format']['structure'].keys():
    print(f"     - {key}")

print(f"\n3️⃣  EXECUTION INSTRUCTIONS:")
print(f"   Game Flow Steps:")
for step, instruction in game_details['instructions']['game_flow'].items():
    print(f"     • {step}: {instruction}")

print(f"\n4️⃣  ACTUAL GAME DATA:")
print(f"   Title: {game_details['game_data']['title']}")
print(f"   Questions: {len(game_details['game_data']['questions'])}")
print(f"   Ready to Execute: ✅ YES")

print()
print()

# ============================================
# Example 7: Generate Complete Learning Module
# ============================================
print("📖 EXAMPLE 7: Generate COMPLETE Learning Module")
print("-" * 70)
print("Request: Create a full lesson with 4 game types")
print()

module_data = {
    "concept": "Cellular Respiration",
    "level": "advanced"
}

print("⏳ Generating module... (this takes 30-40 seconds)")
response = requests.post(f"{API_URL}/generate-module", json=module_data)
module = response.json()["data"]

print(f"✅ Module Generated!")
print(f"\nConcept: {module['concept']}")
print(f"Level: {module['level']}")
print(f"Total Duration: {module['estimated_total_duration']} seconds ({module['estimated_total_duration']//60} minutes)")

print(f"\n📝 Summary:")
print(f"   Title: {module['summary']['title']}")
print(f"   Key Points: {len(module['summary']['key_points'])}")

print(f"\n🎮 Games Included:")
for game_type, game in module['games'].items():
    duration = game.get('estimated_duration', game.get('total_duration', 0))
    print(f"   • {game_type.upper()}: {game['title']}")
    print(f"     Duration: {duration} seconds")

print(f"\n📌 Teaching Notes:")
print(f"   {module['teaching_notes']}")

print()
print()

# ============================================
# Example 8: Get Available Game Types
# ============================================
print("📋 EXAMPLE 8: Learn About All Game Types")
print("-" * 70)

response = requests.get(f"{API_URL}/game-types")
game_types = response.json()["available_games"]

for game in game_types:
    print(f"\n{game['name'].upper()}")
    print(f"   Type: {game['type']}")
    print(f"   Description: {game['description']}")
    print(f"   Best For: {game['best_for']}")
    print(f"   Duration: {game['time_range']}")
    print(f"   User Interaction: {game['interaction']}")

print()
print()

# ============================================
# Example 9: Practical Implementation Pattern
# ============================================
print("💡 EXAMPLE 9: How to Use in Your App")
print("-" * 70)
print("""
STEP 1: Get the game details (format + instructions)
   response = requests.post(
       'http://localhost:8000/game',
       json={'concept': 'Photosynthesis', 'game_type': 'quiz', 'num_questions': 5}
   )
   game_data = response.json()['game_data']
   instructions = response.json()['instructions']

STEP 2: Use the format specification to build your UI
   - Create question elements based on format['structure']
   - Display questions from game_data['questions']
   - Show options from each question

STEP 3: Implement game logic using instructions
   - Follow the game_flow steps from instructions
   - Implement scoring using scoring_rules
   - Track state according to requirements

STEP 4: Display results
   - Calculate final score using scoring_rules
   - Show summary of results
   - Allow replay/next concept

EXAMPLE CODE:
""")

code_example = """
def run_quiz_game(concept, num_questions=5):
    # 1. Get detailed game info
    response = requests.post(
        'http://localhost:8000/game',
        json={
            'concept': concept,
            'game_type': 'quiz',
            'num_questions': num_questions
        }
    )
    
    game = response.json()
    game_data = game['game_data']
    instructions = game['instructions']
    
    # 2. Initialize game
    score = 0
    streak = 0
    questions = game_data['questions']
    
    # 3. Loop through questions
    for question in questions:
        # Display question
        print(f"Q: {question['question']}")
        
        # Display options
        for i, option in enumerate(question['options']):
            print(f"  {i}) {option}")
        
        # Get user answer
        user_answer = int(input("Your answer (0-3): "))
        
        # Check correctness
        if user_answer == question['correct_answer']:
            score += 10  # Base points
            streak += 1
            print(f"✓ Correct! {question['explanation']}")
        else:
            streak = 0
            print(f"✗ Wrong. {question['explanation']}")
        
        # Apply streak multiplier
        if streak > 1:
            score = int(score * 1.5)  # Streak multiplier
    
    # 4. Show final results
    print(f"\\nFinal Score: {score}")
    print(f"Accuracy: {(correct_count/len(questions))*100:.1f}%")

# Run it!
run_quiz_game('Photosynthesis')
"""

print(code_example)

print()
print()

# ============================================
# Summary
# ============================================
print("=" * 70)
print("✅ SUMMARY OF ENDPOINTS")
print("=" * 70)
print("""
/health                 - Check if API is running
/summarize             - Get concept summary (key points, objectives)
/generate-quiz         - Create a multiple-choice quiz
/generate-puzzle       - Create a logic puzzle
/generate-speed        - Create a speed challenge
/game                  - Get complete game details (format + instructions)
/generate-module       - Create a FULL learning module
/game-types            - Learn about all available game types

🎯 RECOMMENDED FLOW:
   1. Use /summarize to introduce concept
   2. Use /game to get instructions for your game type
   3. Use /generate-quiz, /generate-puzzle, or /generate-speed
   4. Implement UI following the game format
   5. Execute game using the instructions provided

💡 PRO TIP:
   Use /game endpoint to get EVERYTHING you need (format, instructions, data)
   Then implement your UI based on the format specification
""")

print()
print("🎉 All examples completed! Check API docs at: http://localhost:8000/docs")
