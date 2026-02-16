"""
Test and Example Usage of the Teacher AI Agent
Run this file to test the AI Agent functionality
"""

import json
import os
from dotenv import load_dotenv
from teacher_ai_agent import TeacherAIAgent

# Load environment variables from .env file
load_dotenv()

def print_header(text):
    """Print formatted header"""
    print(f"\n{'='*60}")
    print(f"  {text}")
    print(f"{'='*60}\n")

def test_concept_summarization():
    """Test concept summarization"""
    print_header("TEST 1: Concept Summarization")
    
    agent = TeacherAIAgent()
    
    concept = "Photosynthesis"
    level = "intermediate"
    
    print(f"📚 Summarizing concept: '{concept}' (Level: {level})")
    print("⏳ Please wait...")
    
    summary = agent.summarize_concept(concept, level)
    
    print(f"\n✅ Summary Generated!")
    print(f"Title: {summary['title']}")
    print(f"\nSummary: {summary['summary']}")
    print(f"\nKey Points:")
    for i, point in enumerate(summary.get('key_points', []), 1):
        print(f"  {i}. {point}")
    
    return summary

def test_quiz_generation():
    """Test quiz game generation"""
    print_header("TEST 2: Quiz Game Generation")
    
    agent = TeacherAIAgent()
    agent.reset_conversation()
    
    concept = "Newton's Laws of Motion"
    num_questions = 3
    
    print(f"🎮 Generating quiz for: '{concept}' ({num_questions} questions)")
    print("⏳ Please wait...")
    
    quiz = agent.generate_quiz_game(concept, num_questions)
    
    print(f"\n✅ Quiz Generated!")
    print(f"Title: {quiz.get('title', 'Quiz')}")
    print(f"Description: {quiz.get('description', 'N/A')}")
    print(f"Number of Questions: {len(quiz.get('questions', []))}")
    
    if quiz.get('questions'):
        print("\nFirst Question:")
        q = quiz['questions'][0]
        print(f"  Q: {q.get('question', 'N/A')}")
        print(f"  Type: {q.get('type', 'N/A')}")
    
    return quiz

def test_puzzle_generation():
    """Test puzzle game generation"""
    print_header("TEST 3: Puzzle Game Generation")
    
    agent = TeacherAIAgent()
    agent.reset_conversation()
    
    concept = "The Water Cycle"
    
    print(f"🧩 Generating puzzle for: '{concept}'")
    print("⏳ Please wait...")
    
    puzzle = agent.generate_puzzle_game(concept)
    
    print(f"\n✅ Puzzle Generated!")
    print(f"Title: {puzzle.get('title', 'Puzzle')}")
    print(f"Type: {puzzle.get('puzzle_type', 'N/A')}")
    print(f"Difficulty: {puzzle.get('difficulty_level', 'N/A')}")
    print(f"Duration: {puzzle.get('estimated_duration', 'N/A')} seconds")
    
    if puzzle.get('content'):
        print(f"Problem: {puzzle['content'].get('problem_statement', 'N/A')[:100]}...")
    
    return puzzle

def test_speed_game_generation():
    """Test speed challenge generation"""
    print_header("TEST 4: Speed Challenge Game Generation")
    
    agent = TeacherAIAgent()
    agent.reset_conversation()
    
    concept = "Basic Multiplication"
    num_challenges = 5
    
    print(f"⚡ Generating speed challenge for: '{concept}' ({num_challenges} challenges)")
    print("⏳ Please wait...")
    
    speed = agent.generate_speed_game(concept, num_challenges)
    
    print(f"\n✅ Speed Challenge Generated!")
    print(f"Title: {speed.get('title', 'Speed Challenge')}")
    print(f"Difficulty: {speed.get('difficulty', 'N/A')}")
    print(f"Number of Challenges: {len(speed.get('challenges', []))}")
    print(f"Total Duration: {speed.get('total_duration', 'N/A')} seconds")
    
    if speed.get('challenges'):
        print("\nFirst Challenge:")
        c = speed['challenges'][0]
        print(f"  Prompt: {c.get('prompt', 'N/A')}")
        print(f"  Time Limit: {c.get('time_limit', 'N/A')} seconds")
    
    return speed

def test_full_learning_module():
    """Test complete learning module generation"""
    print_header("TEST 5: Complete Learning Module Generation")
    
    agent = TeacherAIAgent()
    agent.reset_conversation()
    
    concept = "Cellular Respiration"
    level = "intermediate"
    
    print(f"📖 Generating complete module for: '{concept}' (Level: {level})")
    print("⏳ This will take about 30-40 seconds...")
    print("   - Summarizing concept")
    print("   - Generating quiz game")
    print("   - Generating puzzle game")
    print("   - Generating speed challenge")
    
    module = agent.generate_full_learning_module(concept, level)
    
    print(f"\n✅ Learning Module Generated!")
    print(f"Concept: {module['concept']}")
    print(f"Level: {module['level']}")
    print(f"\nSummary Title: {module['summary']['title']}")
    print(f"Key Points: {len(module['summary'].get('key_points', []))}")
    
    print(f"\nGames Included:")
    for game_type, game_data in module['games'].items():
        print(f"  ✓ {game_type.capitalize()}: {game_data.get('title', f'{game_type} game')}")
    
    print(f"\nTotal Estimated Duration: {module['estimated_total_duration']} seconds")
    print(f"                      ({module['estimated_total_duration']/60:.1f} minutes)")
    
    return module

def save_module_to_file(module, filename="generated_module.json"):
    """Save generated module to a JSON file"""
    with open(filename, 'w') as f:
        json.dump(module, f, indent=2)
    print(f"✅ Module saved to {filename}")

def main():
    """Run all tests"""
    print("\n" + "="*60)
    print("  Teacher AI Agent - Test Suite")
    print("="*60)
    print("\nThis script demonstrates the capabilities of the Teacher AI Agent")
    print("Each test will call the AI to generate different types of content")
    
    try:
        # Run tests
        results = {}
        
        # Test 1: Summarization
        results['summary'] = test_concept_summarization()
        
        input("\n\nPress ENTER to continue to Test 2 (Quiz Generation)...")
        results['quiz'] = test_quiz_generation()
        
        input("\n\nPress ENTER to continue to Test 3 (Puzzle Generation)...")
        results['puzzle'] = test_puzzle_generation()
        
        input("\n\nPress ENTER to continue to Test 4 (Speed Challenge)...")
        results['speed'] = test_speed_game_generation()
        
        input("\n\nPress ENTER to continue to Test 5 (Complete Module)...")
        results['module'] = test_full_learning_module()
        
        # Summary
        print_header("✅ All Tests Completed Successfully!")
        print("\nGenerated Content:")
        print("  ✓ Concept Summary")
        print("  ✓ Quiz Game")
        print("  ✓ Puzzle Game")
        print("  ✓ Speed Challenge")
        print("  ✓ Complete Learning Module")
        
        # Save the module
        save_module_to_file(results['module'])
        
        print("\n📝 You can now:")
        print("  1. Review the generated_module.json file")
        print("  2. Use the client.py to integrate with your backend")
        print("  3. Use TeacherAIClient.js to integrate with React")
        print("  4. Check example_react_integration.jsx for frontend examples")
        
    except KeyboardInterrupt:
        print("\n\n⚠️  Tests interrupted by user")
    except Exception as e:
        print(f"\n❌ Error during testing: {e}")
        print("Make sure:")
        print("  1. You have set your OPENAI_API_KEY in .env")
        print("  2. You have internet connectivity")
        print("  3. Your API key is valid")

if __name__ == "__main__":
    main()
