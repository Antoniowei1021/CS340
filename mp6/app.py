from flask import Flask, render_template, request, jsonify
import os
import requests
from datetime import datetime, timedelta
from dateutil import parser

app = Flask(__name__)

weather_cache = {}
cache_expiry = datetime.now()  # Initializing with the current time means it's immediately expired
CACHE_DURATION = timedelta(hours=1)

@app.route('/')
def index():
    return render_template("index.html")
@app.route('/weather', methods=["POST"])
def POST_weather():
    global weather_cache, cache_expiry
    week_dic = {
        'M': 0,
        'T': 1,
        'W': 2,
        'R': 3,
        'F': 4   
    }
    major = ""
    number = ""
    course = request.form["course"]
    course = course.replace(" ", "")
    for i in course:
        if i.isdigit():
            number += i
        elif i.isalpha():
            major += i.upper()
    if len(number) > 3:
        return "Invalid course format", 400
    server_url = os.getenv('COURSES_MICROSERVICE_URL')
    response = requests.get(f"{server_url}/{major}/{number}")
    if response.status_code != 200:
        return "Course not found", 400
    r = response.json()
    dia = r['Days of Week']
    characters = list(dia)
    days = [week_dic[c] for c in characters]
    start_t = r['Start Time']
    in_time = datetime.strptime(start_t, "%I:%M %p")
    start_t = datetime.strftime(in_time, "%H:%M:%S")
    week_day = datetime.now().weekday()
    current_datetime = datetime.now()
    current_date = current_datetime.date()
    next_meeting_datetime = None
    for day in sorted(days):
        if day > week_day:
            next_meeting_date = current_date + timedelta(days=day-week_day)
            next_meeting_datetime = datetime.combine(next_meeting_date, datetime.strptime(start_t, "%H:%M:%S").time())
            break
    if not next_meeting_datetime:
        day = days[0]
        next_meeting_date = current_date + timedelta(days=(7-week_day) + day)
        next_meeting_datetime = datetime.combine(next_meeting_date, datetime.strptime(start_t, "%H:%M:%S").time())
    forecast_lookup_datetime = next_meeting_datetime.replace(minute=0, second=0)
    weather = requests.get("https://api.weather.gov/gridpoints/ILX/96,72/forecast/hourly").json()
    forecast = None
    for hours in weather["properties"]["periods"]:
        weather_datetime = hours['startTime'].replace("-05:00", "")
        weather_datetime = datetime.strptime(weather_datetime, "%Y-%m-%dT%H:%M:%S")
        if weather_datetime == forecast_lookup_datetime:
            forecast = {
                "temperature": hours['temperature'],
                "shortForecast": hours['shortForecast']
            }
            break
    if not forecast:
        forecast = {
            "temperature": "forecast unavailable",
            "shortForecast": "forecast unavailable"
        }
    data = {
        "course": f"{major} {number}",
        "nextCourseMeeting": str(next_meeting_datetime),
        "forecastTime": str(forecast_lookup_datetime), 
        **forecast
    }
    weather_cache = data
    cache_expiry = datetime.now() + CACHE_DURATION
    return jsonify(data)
@app.route('/weatherCache')
def get_cached_weather():
    if datetime.now() < cache_expiry and weather_cache:
        return jsonify(weather_cache)
    return jsonify({})
