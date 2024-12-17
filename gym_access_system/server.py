from flask import Flask, jsonify, request, send_from_directory

app = Flask(__name__)

# Set of valid users
valid_users = {
    "iulia",  # Example user
}

# Set to track who is currently in the gym
people_in_gym = set()

# Fan control status
fan_on = False

@app.route('/')
def serve_index():
    """Serve the HTML file."""
    return send_from_directory('static', 'index.html')

@app.route('/api/entry_exit', methods=['POST'])
def entry_exit():
    """Endpoint to handle entry or exit based on name."""
    name = request.json.get("name")

    if name in valid_users:
        if name not in people_in_gym:
            people_in_gym.add(name)
            return jsonify({"status": "success", "message": "Access granted, member entered", "fan": fan_on}), 200
        else:
            people_in_gym.remove(name)
            return jsonify({"status": "success", "message": "Member exited", "fan": fan_on}), 200
    else:
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


@app.route('/api/toggle_fan', methods=['POST'])
def toggle_fan():
    """Endpoint to explicitly toggle the fan ON/OFF."""
    global fan_on

    # Toggle fan status
    fan_on = not fan_on
    return jsonify({"status": "success", "fan_on": fan_on}), 200


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8000)
