from google import genai
import pyttsx3
import threading
import speech_recognition as sr
from faster_whisper import WhisperModel
import io
import tempfile
import os
import queue
import requests
import logging
import time
import sys
from xml.etree import ElementTree as ET

LOG_FILE = "data.log"

logging.basicConfig(
    filename=LOG_FILE,
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s"
)

engine = pyttsx3.init()
engine.setProperty('rate', 150)
engine.setProperty('volume', 0.9)

temp_dir = tempfile.mkdtemp()
temp_audio_file = os.path.join(temp_dir, "temp_audio.wav")

model = WhisperModel("base", device="cpu", compute_type="int8")

client = genai.Client(api_key="AIzaSyCp5a4gbFYJm4l6_3XpDhsUVAL3CwJaV-8")

r = sr.Recognizer()

is_listening = False
listening_thread = None
is_voice_mode = False
is_clear_mode = False
exit_ai = False
stop_speaking = False

processing_lock = threading.Lock()
speak_lock = threading.Lock()
response_queue = queue.Queue()

def clear_terminal():
    os.system('cls' if os.name == 'nt' else 'clear')

def delete_log():
    try:
        for handler in logging.root.handlers[:]:
            handler.close()
            logging.root.removeHandler(handler)
        if os.path.exists(LOG_FILE):
            os.remove(LOG_FILE)
            print("Data deleted successfully.")
        else:
            print("No log data found. Creating a new one...")
        logging.basicConfig(
            filename=LOG_FILE,
            level=logging.INFO,
            format="%(asctime)s - %(levelname)s - %(message)s"
        )
        logging.info("Logging system initialized/reinitialized after log deletion.")
    except Exception as e:
        print(f"[!] Error deleting or reopening log: {e}")

def get_live_news():
    try:
        url = "https://news.google.com/rss?hl=en&gl=US&ceid=US:en"
        response = requests.get(url, timeout=5)
        root = ET.fromstring(response.content)
        items = root.findall(".//item")
        headlines = [item.find("title").text for item in items[:3]]
        if headlines:
            return "Latest global news headlines: " + "; ".join(headlines)
        else:
            return "Sorry, I couldn't find any news headlines right now."
    except Exception as e:
        logging.error(f"News fetch error: {e}")
        return "Sorry, I couldn't fetch the latest news."

def get_weather(location="Bokaro Steel City"):
    try:
        url = f"https://wttr.in/{location}?format=3"
        response = requests.get(url, timeout=5)
        if response.status_code == 200:
            return f"Current weather in {location}: {response.text.strip()}"
        else:
            return "Sorry, I couldn't fetch the weather information."
    except Exception as e:
        logging.error(f"Weather fetch error: {e}")
        return "Sorry, I couldn't fetch the weather information."

def get_realtime_web_data(query):
    try:
        url = "https://api.duckduckgo.com/"
        params = {"q": query, "format": "json", "no_html": 1, "skip_disambig": 1}
        response = requests.get(url, params=params, timeout=5)
        data = response.json()
        results = []
        if "RelatedTopics" in data:
            for topic in data["RelatedTopics"][:3]:
                if "Text" in topic:
                    results.append(topic["Text"])
        if results:
            return "Here are some live web results: " + "; ".join(results)
        else:
            return "I couldn’t find anything relevant on the web."
    except Exception as e:
        logging.error(f"Web search error: {e}")
        return "Sorry, I couldn’t retrieve live web data right now."

def type_text_slowly(text, delay=0.03):
    for char in text:
        sys.stdout.write(char)
        sys.stdout.flush()
        time.sleep(delay)
    print()

def speak_text(text):
    global stop_speaking
    stop_speaking = False
    with speak_lock:
        try:
            engine.say(text)
            engine.runAndWait()
        except Exception as e:
            logging.error(f"TTS error: {e}")

def stop_voice_playback():
    global stop_speaking
    stop_speaking = True
    engine.stop()
    print("Voice playback stopped.")
    logging.info("Voice playback stopped by user.")

def chatbot_response(prompt, voice_mode, clear_mode):
    try:
        if prompt.lower().startswith("weather"):
            parts = prompt.split(maxsplit=1)
            location = parts[1] if len(parts) > 1 else "Bokaro Steel City"
            response_text = get_weather(location)
        elif "news" in prompt.lower() or "headline" in prompt.lower():
            response_text = get_live_news()
        elif prompt.lower().startswith("search") or prompt.lower().startswith("web"):
            query = prompt.split(maxsplit=1)[1] if len(prompt.split()) > 1 else ""
            if query:
                response_text = get_realtime_web_data(query)
            else:
                response_text = "Please tell me what to search for."
        elif prompt.lower() in ["hi", "hello", "hey"]:
            response_text = "Hello there! How can I help you today?"
        else:
            full_prompt = prompt
            start_time = time.time()
            logging.info(f"User Prompt: {prompt}")
            response = client.models.generate_content(
                model="gemini-2.5-flash",
                contents=full_prompt
            )
            response_text = getattr(response, "text", str(response))
            elapsed = time.time() - start_time
            logging.info(f"AI Response: {response_text}")
            logging.info(f"Response time: {elapsed:.2f}s")
        if clear_mode:
            clear_terminal()
        print("\nAI is typing...")
        type_text_slowly(response_text)
        if voice_mode:
            threading.Thread(target=speak_text, args=(response_text,), daemon=True).start()
    except Exception as e:
        logging.error(f"Response generation error: {e}")
        print(f"[!] Error generating response: {e}")

def transcribe_realtime_whisper():
    global is_listening
    print("Listening... Speak now.")
    logging.info("Voice recognition started.")
    try:
        with sr.Microphone(sample_rate=16000) as source:
            r.adjust_for_ambient_noise(source, duration=0.5)
            print("Microphone calibrated. Listening...")
            while is_listening:
                try:
                    print(".", end="", flush=True)
                    audio = r.listen(source, timeout=1, phrase_time_limit=10)
                    with processing_lock:
                        audio_data = io.BytesIO(audio.get_wav_data())
                        with open(temp_audio_file, "wb") as f:
                            f.write(audio_data.read())
                        segments, info = model.transcribe(temp_audio_file, beam_size=5, language="en")
                        transcribed_text = " ".join([segment.text for segment in segments]).strip()
                        if transcribed_text and len(transcribed_text.split()) > 1:
                            print(f"\n🗣 You said: {transcribed_text}\n")
                            logging.info(f"Voice input transcribed: {transcribed_text}")
                            threading.Thread(
                                target=chatbot_response,
                                args=(transcribed_text, is_voice_mode, is_clear_mode),
                                daemon=True
                            ).start()
                except sr.WaitTimeoutError:
                    pass
                except Exception as e:
                    logging.error(f"Speech recognition error: {e}")
                    print(f"[!] Error during speech recognition: {e}")
    except Exception as mic_error:
        logging.error(f"Microphone access failed: {mic_error}")
        print(f"[!] Failed to access microphone: {mic_error}")

def main():
    global is_listening, listening_thread, is_voice_mode, is_clear_mode, exit_ai
    while not exit_ai:
        while not response_queue.empty():
            prompt, voice_mode = response_queue.get()
            chatbot_response(prompt, voice_mode, is_clear_mode)
        print("\n--- AI Menu ---")
        print("1: Exit AI|2: Enable Voice Mode|3: Disable Voice Mode|4: Enable Voice Recognition|")
        print("5: Disable Voice Recognition|6: Enable Clear Terminal Mode|7: Disable Clear Terminal|Type 'del_data' delete previous data")
        print("-----------------------------")
        prompt = input("Your prompt: ").strip()
        logging.info(f"Menu Input: {prompt}")
        if prompt == "1":
            exit_ai = True
            is_listening = False
            print("Left the AI. Thanks for using it!")
            logging.info("AI session exited by user.")
            if listening_thread and listening_thread.is_alive():
                listening_thread.join()
        elif prompt == "2":
            is_voice_mode = True
            print("Voice mode enabled.")
            logging.info("Voice mode enabled.")
        elif prompt == "3":
            is_voice_mode = False
            print("Voice mode disabled.")
            logging.info("Voice mode disabled.")
        elif prompt == "4":
            if not is_listening:
                is_listening = True
                listening_thread = threading.Thread(target=transcribe_realtime_whisper, daemon=True)
                listening_thread.start()
                print("Voice recognition started.")
                logging.info("Voice recognition enabled.")
            else:
                print("Voice recognition is already active!")
                logging.info("Voice recognition start ignored (already active).")
        elif prompt == "5":
            if is_listening:
                is_listening = False
                print("Voice recognition stopped.")
                logging.info("Voice recognition disabled.")
            else:
                print("Voice recognition is not active!")
                logging.info("Voice recognition stop ignored (already inactive).")
        elif prompt == "6":
            is_clear_mode = True
            print("Clear Terminal Mode ON")
            logging.info("Clear Terminal Mode enabled.")
        elif prompt == "7":
            is_clear_mode = False
            print("Clear Terminal Mode OFF")
            logging.info("Clear Terminal Mode disabled.")
        elif prompt == "8":
            stop_voice_playback()
        elif prompt.lower() == "del_data":
            delete_log()
        else:
            chatbot_response(prompt, is_voice_mode, is_clear_mode)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nExiting due to keyboard interrupt...")
        logging.info("AI session exited by keyboard interrupt.")
        if listening_thread and listening_thread.is_alive():
            is_listening = False
            listening_thread.join()
        sys.exit(0)
