from google import genai
import pyttsx3
import threading

# Initialize Gemini client
client = genai.Client()

# Initialize TTS engine
engine = pyttsx3.init()
engine.setProperty('rate', 150)
engine.setProperty('volume', 0.9)

# Lock for thread-safe speech
speak_lock = threading.Lock()

def speak_text(text):
    with speak_lock:
        engine.say(text)
        engine.runAndWait()

def chatbot_response(prompt, is_voice_mode):
    try:
        response = client.models.generate_content(
            model="gemini-2.5-flash",
            contents=prompt
        )
        print("\nAI:", response.text)
        if is_voice_mode:
            threading.Thread(target=speak_text, args=(response.text,)).start()
    except Exception as e:
        print("Error communicating with the AI:", e)

exit_ai = 0
is_voice_mode = False

while exit_ai == 0:
    print("\n--- AI Chatbot Menu ---")
    print("Enter 1 to leave the AI")
    print("Enter 2 to enter voice mode for AI")
    print("Enter 3 to exit voice mode for AI")
    prompt = input("Your message to the AI (or 1/2/3): ").strip()

    if prompt == "1":
        exit_ai = 1
        print("Left the AI. Thanks for using it.")
    elif prompt == "2":
        is_voice_mode = True
        print("You have entered voice mode for AI.")
    elif prompt == "3":
        is_voice_mode = False
        print("You have exited voice mode for AI.")
    else:
        chatbot_response(prompt, is_voice_mode)
