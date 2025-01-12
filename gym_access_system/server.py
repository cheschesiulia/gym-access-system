from flask import Flask, jsonify, request, send_from_directory
from datetime import datetime, timedelta

app = Flask(__name__)

# Set of valid users
valid_users = {
    "iulia",  # Example user
}

# Set to track who is currently in the gym
people_in_gym = set()

# Fan control status
fan_on = False

# List to store access logs
access_logs = []

# List to store hourly count (for 24 hours)
hourly_count = [0] * 24  # 24 hours in a day

# Dictionary to store entry times of users (using name as the key)
entry_times = {}

@app.route('/')
def serve_index():
    """Serve the HTML file."""
    return send_from_directory('static', 'index.html')

@app.route('/api/entry_exit', methods=['POST'])
def entry_exit():
    """Endpoint to handle entry or exit based on name."""
    name = request.json.get("name")
    timestamp = datetime.now()
    hour = timestamp.hour  # Extract the hour part of the timestamp

    if name in valid_users:
        if name not in people_in_gym:
            # Log the entry time
            people_in_gym.add(name)
            entry_times[name] = timestamp  # Save entry time
            # Log the entry
            access_logs.append(f"{timestamp.strftime('%Y-%m-%d %H:%M:%S')} - {name} entered the gym.")
            # Increment the hourly count for the current hour
            hourly_count[hour] += 1
            return jsonify({"status": "success", "message": "Access granted, member entered", "fan": fan_on}), 200
        else:
            # Log the exit time and update hourly count for the exit time
            entry_time = entry_times.get(name)
            exit_hour = timestamp.hour  # Hour of exit time

            # Calculate hours spent in the gym and update hourly count
            if entry_time:
                entry_hour = entry_time.hour
                for h in range(entry_hour, exit_hour + 1):
                    hourly_count[h] += 1  # Increment count for each hour between entry and exit

            # Log the exit
            people_in_gym.remove(name)
            access_logs.append(f"{timestamp.strftime('%Y-%m-%d %H:%M:%S')} - {name} exited the gym.")
            # Decrement the hourly count for the exit hour
            hourly_count[exit_hour] -= 1
            return jsonify({"status": "success", "message": "Member exited", "fan": fan_on}), 200
    else:
        # Log the failed entry attempt
        access_logs.append(f"{timestamp.strftime('%Y-%m-%d %H:%M:%S')} - {name} tried to enter, no valid membership found.")
        return jsonify({"status": "error", "message": "Access denied, invalid membership"}), 200

@app.route('/api/people_count', methods=['GET'])
def get_people_count():
    """Endpoint to get the number of people currently inside."""
    return jsonify({"people_count": len(people_in_gym)}), 200

@app.route('/api/get_fan_status', methods=['GET'])
def get_fan_status():
    """Endpoint to get the current status of the fan plus changing its state if there are more than 5 people in the gym."""
    global fan_on

    if len(people_in_gym) == 5:
        fan_on = True
    elif len(people_in_gym) == 4:
        fan_on = False

    return jsonify({"fan_on": fan_on}), 200

@app.route('/api/toggle_fan', methods=['POST'])
def toggle_fan():
    """Endpoint to explicitly toggle the fan ON/OFF."""
    global fan_on

    # Get the new fan status from the request (if provided)
    data = request.get_json()
    if 'fan_on' in data:
        fan_on = data['fan_on']

    return jsonify({"status": "success", "fan_on": fan_on}), 200

@app.route('/api/access_logs', methods=['GET'])
def get_access_logs():
    """Endpoint to retrieve the real-time logs of people entering/exiting the gym."""
    return jsonify({"logs": access_logs}), 200

@app.route('/api/hourly_count', methods=['GET'])
def get_hourly_count():
    """Endpoint to retrieve the number of people in the gym for each hour of the current day."""
    return jsonify({"hourly_count": hourly_count}), 200

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8000)
