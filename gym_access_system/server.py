from flask import Flask, jsonify, request, send_from_directory
from datetime import datetime

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

@app.route('/')
def serve_index():
    """Serve the HTML file."""
    return send_from_directory('static', 'index.html')

@app.route('/api/entry_exit', methods=['POST'])
def entry_exit():
    """Endpoint to handle entry or exit based on name."""
    name = request.json.get("name")

    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')  # Get current timestamp
    
    if name in valid_users:
        if name not in people_in_gym:
            people_in_gym.add(name)
            # Log the entry
            access_logs.append(f"{timestamp} - {name} entered the gym.")
            return jsonify({"status": "success", "message": "Access granted, member entered", "fan": fan_on}), 200
        else:
            people_in_gym.remove(name)
            # Log the exit
            access_logs.append(f"{timestamp} - {name} exited the gym.")
            return jsonify({"status": "success", "message": "Member exited", "fan": fan_on}), 200
    else:
        # Log the failed entry attempt
        access_logs.append(f"{timestamp} - {name} tried to enter, no valid membership found.")
        return jsonify({"status": "error", "message": "Access denied, invalid membership"}), 200


@app.route('/api/people_count', methods=['GET'])
def get_people_count():
    """Endpoint to get the number of people currently inside."""
    return jsonify({"people_count": len(people_in_gym)}), 200


@app.route('/api/control_fan', methods=['POST'])
def control_fan():
    """Endpoint to control the fan automatically based on the number of people."""
    global fan_on

    # Example logic: Turn fan ON if more than 5 people are inside
    fan_on = len(people_in_gym) > 5

    return jsonify({"status": "success", "fan_on": fan_on}), 200

@app.route('/api/get_fan_status', methods=['GET'])
def get_fan_status():
    """Endpoint to get the current status of the fan."""
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


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8000)
