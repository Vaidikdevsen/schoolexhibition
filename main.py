from google import genai
import pyttsx3
import threading

engine = pyttsx3.init()

rate = engine.getProperty('rate')
engine.setProperty('rate', 150)
volume = engine.getProperty('volume')
engine.setProperty('volume', 0.9)





speak_lock = threading.Lock()

def speak_text(text):
    """Function to speak text using a lock to prevent race conditions."""
    with speak_lock:
        engine.say(text)
        engine.runAndWait()

def chatbot_response(prompt,is_voice_mode):

    response = client.models.generate_content(
    model="gemini-2.5-flash",
    contents=prompt
    )


    print(response.text)

    if is_voice_mode == True:
        threading.Thread(target=speak_text, args=(response.text,)).start()

    


client = genai.Client()
exit_ai = 0
is_voice_mode = False

while exit_ai==0:
    print("\n--- AI Chatbot Menu ---")
    print("Enter 1 to leave the AI")
    print("Enter 2 to enter voice mode for AI")
    print("Enter 3 to exit voice mode for AI")
    prompt = input("Enter a prompt for the AI: ")
    if prompt == "1":
        exit_ai = 1
        print("Left the AI. Thanks for using it.")
        
            
    
    elif prompt == "2":
        is_voice_mode = True
        print("You have entered voice mode for AI. Enter 3 to exit voice mode.")
    
    elif prompt == "3":
        is_voice_mode = False
        print("You have exited voice mode for AI.")
        
    else:
        
        chatbot_response(prompt, is_voice_mode)
