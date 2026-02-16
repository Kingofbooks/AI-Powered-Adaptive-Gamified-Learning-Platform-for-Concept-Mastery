"""
QUICK REFERENCE GUIDE - Teacher AI Agent API

Copy and paste these snippets directly into your code!
"""

# ============================================
# QUICK COPY-PASTE SNIPPETS
# ============================================

# SNIPPET 1: Check if API is running
import requests

def check_api():
    """Check if the API server is running"""
    try:
        response = requests.get("http://localhost:8000/health")
        print("✅ API is running!")
        return True
    except:
        print("❌ API is not running. Start with: python main.py")
        return False


# SNIPPET 2: Get a concept summary
def get_concept_summary(concept, level="beginner"):
    """
    Get a summary of any concept with key points and learning objectives
    
    Args:
        concept: What to learn about (e.g., "Photosynthesis")
        level: "beginner", "intermediate", or "advanced"
    
    Returns:
        Dictionary with title, summary, key_points, learning_objectives
    """
    response = requests.post(
        "http://localhost:8000/summarize",
        json={"concept": concept, "level": level}
    )
    return response.json()["data"]


# SNIPPET 3: Generate a quiz game
def generate_quiz(concept, num_questions=5, level="intermediate"):
    """
    Generate a multiple-choice quiz about any concept
    
    Args:
        concept: What to quiz about (e.g., "Newton's Laws")
        num_questions: How many questions (1-20)
        level: "beginner", "intermediate", or "advanced"
    
    Returns:
        Dictionary with questions, scoring rules, estimated duration
    """
    response = requests.post(
        "http://localhost:8000/generate-quiz",
        json={
            "concept": concept,
            "game_type": "quiz",
            "num_questions": num_questions,
            "level": level
        }
    )
    return response.json()["data"]


# SNIPPET 4: Generate a puzzle game
def generate_puzzle(concept, level="intermediate"):
    """
    Generate a logic puzzle about any concept
    
    Args:
        concept: What the puzzle is about
        level: "beginner", "intermediate", or "advanced"
    
    Returns:
        Dictionary with problem statement, answer, hints, explanation
    """
    response = requests.post(
        "http://localhost:8000/generate-puzzle",
        json={"concept": concept, "level": level}
    )
    return response.json()["data"]


# SNIPPET 5: Generate a speed challenge
def generate_speed_challenge(concept, num_questions=10, level="intermediate"):
    """
    Generate timed rapid-fire challenges
    
    Args:
        concept: What to challenge about (e.g., "Multiplication")
        num_questions: How many challenges (5-20)
        level: "beginner", "intermediate", or "advanced"
    
    Returns:
        Dictionary with challenges, time limits, scoring rules
    """
    response = requests.post(
        "http://localhost:8000/generate-speed",
        json={
            "concept": concept,
            "game_type": "speed",
            "num_questions": num_questions,
            "level": level
        }
    )
    return response.json()["data"]


# SNIPPET 6: Get complete game details (BEST FOR IMPLEMENTATION)
def get_game_details(concept, game_type="quiz", level="intermediate", num_questions=5):
    """
    Get EVERYTHING needed to build and run a game:
    - Learning objectives
    - Game format specification (JSON structure)
    - Execution instructions
    - Ready-to-use game data
    
    This is the RECOMMENDED endpoint to use!
    
    Args:
        concept: What to learn
        game_type: "quiz", "puzzle", or "speed"
        level: "beginner", "intermediate", or "advanced"
        num_questions: How many questions
    
    Returns:
        Dictionary with:
            - objectives: What students will learn
            - format: JSON structure of the game
            - instructions: Step-by-step how to run the game
            - game_data: The actual game ready to execute
    """
    response = requests.post(
        "http://localhost:8000/game",
        json={
            "concept": concept,
            "game_type": game_type,
            "level": level,
            "num_questions": num_questions
        }
    )
    return response.json()


# SNIPPET 7: Get all available game types
def get_game_types():
    """
    See what game types are available and when to use each
    
    Returns:
        List of game types with descriptions
    """
    response = requests.get("http://localhost:8000/game-types")
    return response.json()["available_games"]


# SNIPPET 8: Generate a complete learning module
def generate_complete_module(concept, level="intermediate"):
    """
    Generate a FULL learning experience with all 4 game types:
    1. Summary
    2. Quiz
    3. Puzzle
    4. Speed Challenge
    
    Note: This takes 30-40 seconds to generate
    
    Args:
        concept: The main concept to learn
        level: "beginner", "intermediate", or "advanced"
    
    Returns:
        Complete module with all games + teaching notes
    """
    print("⏳ Generating complete module (30-40 seconds)...")
    response = requests.post(
        "http://localhost:8000/generate-module",
        json={
            "concept": concept,
            "level": level
        }
    )
    print("✅ Module ready!")
    return response.json()["data"]


# ============================================
# PRACTICAL USAGE EXAMPLES
# ============================================

# EXAMPLE 1: Simple quiz implementation
def run_simple_quiz():
    """Load a quiz and display it"""
    quiz = generate_quiz("Photosynthesis", num_questions=3)
    
    print(f"Quiz: {quiz['title']}")
    print(f"Questions: {len(quiz['questions'])}")
    
    for i, question in enumerate(quiz['questions'], 1):
        print(f"\n{i}. {question['question']}")
        for j, option in enumerate(question['options']):
            print(f"   {j}) {option}")


# EXAMPLE 2: Run game with game details (BEST PRACTICE)
def run_game_with_details():
    """
    Get game details and implement based on format specification
    This is the RECOMMENDED approach!
    """
    # Step 1: Get all the information
    game_info = get_game_details(
        concept="Water Cycle",
        game_type="quiz",
        level="intermediate",
        num_questions=3
    )
    
    # Step 2: Extract what we need
    game_data = game_info['game_data']
    instructions = game_info['instructions']
    format_spec = game_info['format']
    objectives = game_info['objectives']
    
    # Step 3: Display objectives to student
    print("🎯 Learning Goals:")
    for obj in objectives['learning_objectives']:
        print(f"   • {obj}")
    
    # Step 3: Run the game following instructions
    print(f"\n📖 {instructions['game_flow']['start_message']}")
    
    score = 0
    for question in game_data['questions']:
        print(f"\nQ: {question['question']}")
        for j, option in enumerate(question['options']):
            print(f"   {j}) {option}")
        
        user_answer = int(input("Your answer: "))
        if user_answer == question['correct_answer']:
            score += 10
            print(f"✓ Correct! {question['explanation']}")
        else:
            print(f"✗ Wrong. {question['explanation']}")
    
    print(f"\n🏆 Final Score: {score}")


# EXAMPLE 3: Speed challenge timer
def run_speed_challenge():
    """Run a speed challenge with time limits"""
    import time
    
    speed = generate_speed_challenge("Basic Math", num_questions=3)
    
    print(f"Challenge: {speed['title']}")
    print(f"⏰ You have {speed['total_duration']} seconds total\n")
    
    start_time = time.time()
    correct = 0
    
    for challenge in speed['challenges']:
        print(f"Time limit: {challenge['time_limit']} seconds")
        print(f"Q: {challenge['prompt']}")
        user_answer = input("Answer: ")
        
        if user_answer.strip() == str(challenge['answer']):
            correct += 1
            print("✓ Correct!")
        else:
            print(f"✗ Wrong. Answer was: {challenge['answer']}")
        print()
    
    total_time = time.time() - start_time
    accuracy = (correct / len(speed['challenges'])) * 100
    
    print(f"📊 Results:")
    print(f"   Accuracy: {accuracy:.1f}%")
    print(f"   Time taken: {total_time:.1f} seconds")


# EXAMPLE 4: Get summary and quiz for a concept
def learn_complete_topic(concept):
    """Full learning experience: summary → quiz"""
    print(f"📚 Learning: {concept}\n")
    
    # Get summary
    summary = get_concept_summary(concept, level="intermediate")
    print(f"Summary: {summary['summary']}\n")
    print("Key Points:")
    for point in summary['key_points']:
        print(f"  • {point}")
    
    # Generate quiz
    print("\n" + "="*50)
    quiz = generate_quiz(concept, num_questions=3)
    print(f"\n📝 Quiz: {quiz['title']}\n")
    
    for question in quiz['questions']:
        print(f"Q: {question['question']}")
        for j, opt in enumerate(question['options']):
            print(f"   {j}) {opt}")
        print(f"Answer: {chr(65 + question['correct_answer'])}")
        print()


# ============================================
# ERROR HANDLING
# ============================================

def safe_api_call(func, *args, **kwargs):
    """
    Safely call any API function with error handling
    
    Usage:
        result = safe_api_call(generate_quiz, "Photosynthesis", num_questions=5)
    """
    try:
        return func(*args, **kwargs)
    except requests.exceptions.ConnectionError:
        print("❌ Cannot connect to API. Is the server running? (python main.py)")
        return None
    except requests.exceptions.Timeout:
        print("❌ API is taking too long to respond. Try again.")
        return None
    except Exception as e:
        print(f"❌ Error: {str(e)}")
        return None


# ============================================
# QUICK START
# ============================================

if __name__ == "__main__":
    print("🚀 QUICK START EXAMPLES\n")
    
    # Check API
    if not check_api():
        exit()
    
    # Try 1 example (uncomment the one you want):
    
    # run_simple_quiz()
    # run_game_with_details()
    # run_speed_challenge()
    # learn_complete_topic("Photosynthesis")
    
    print("\n✅ Ready! Uncomment an example above to run it.")
