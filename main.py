from google import genai
import pyttsx3
import threading
import speech_recognition as sr
from faster_whisper import WhisperModel
import io
import tempfile
import os
import queue

engine = pyttsx3.init()

rate = engine.getProperty('rate')
engine.setProperty('rate', 150)
volume = engine.getProperty('volume')
engine.setProperty('volume', 0.9)

temp_dir = tempfile.mkdtemp()
temp_audio_file = os.path.join(temp_dir, "temp_audio.wav")


model_size = "base"
model = WhisperModel(model_size, device="cpu", compute_type="int8")


r = sr.Recognizer()

is_listening = False
listening_thread = None
processing_lock = threading.Lock()
response_queue = queue.Queue()


def transcribe_realtime_whisper():
    global is_listening
    print("Listening... Speak now.")
    
    with sr.Microphone(sample_rate=16000) as source:
        r.adjust_for_ambient_noise(source, duration=0.5) 
        
        while is_listening:  
            try:
                audio = r.listen(source, timeout=0.5, phrase_time_limit=10)  
                
                with processing_lock:
                    audio_data = io.BytesIO(audio.get_wav_data())
                    
                    with open(temp_audio_file, "wb") as f:
                        f.write(audio_data.read())
                    
                    segments, info = model.transcribe(temp_audio_file, beam_size=5, language="en")
                    transcribed_text = " ".join([segment.text for segment in segments]).strip()
                    
                    if transcribed_text and len(transcribed_text.split()) > 1:
                        threading.Thread(target=chatbot_response, args=(transcribed_text, is_voice_mode), daemon=True).start()
            
            except sr.WaitTimeoutError:
                pass
            except Exception as e:
                if is_listening:
                    pass


speak_lock = threading.Lock()

def speak_text(text):
    with speak_lock:
        engine.say(text)
        engine.runAndWait()

is_voice_mode = False

def chatbot_response(prompt, is_voice_mode):
    response = client.models.generate_content(
        model="gemini-2.5-flash",
        contents=prompt
    )
    
    print(f"AI: {response.text}")

    if is_voice_mode:
        threading.Thread(target=speak_text, args=(response.text,)).start()


client = genai.Client()
exit_ai = 0

while exit_ai == 0:
    if not response_queue.empty():
        prompt, voice_mode = response_queue.get()
        chatbot_response(prompt, voice_mode)
    

    print("\n--- AI Chatbot Menu ---")
    print("Enter 1 to leave the AI")
    print("Enter 2 to enter voice mode for AI")
    print("Enter 3 to exit voice mode for AI")
    print("Enter 4 to use voice recognition mode to let AI understand your voice")
    print("Enter 5 to exit voice recognition mode to understand your voice")

    prompt = input("Enter a prompt for the AI: \n")
    
    if prompt == "1":
        exit_ai = 1
        is_listening = False
        print("Left the AI. Thanks for using it.")
    
    elif prompt == "2":
        is_voice_mode = True
        print("You have entered voice mode for AI. Enter 3 to exit voice mode.")
    
    elif prompt == "3":
        is_voice_mode = False
        print("You have exited voice mode for AI.")

    elif prompt == "4":  
        if not is_listening:
            is_listening = True
            listening_thread = threading.Thread(target=transcribe_realtime_whisper, daemon=True)
            listening_thread.start()
            print("Voice recognition started. Speak to interact with AI.")
        else:
            print("Voice recognition is already active!")
    
    elif prompt == "5":  
        if is_listening:
            is_listening = False
            print("Voice recognition stopped.")
        else:
            print("Voice recognition is not active!")
    
    else:
        chatbot_response(prompt, is_voice_mode)
