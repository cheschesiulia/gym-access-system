from flask import Flask, jsonify, request
from time import time

app = Flask(__name__)

# Set of valid users (now contains names)
valid_users = {
    "iulia",  # Name for valid user
    # Add more names as needed
}

# Set to track who is currently in the gym (names of people inside)
people_in_gym = set()

# Fan control status (mock)
fan_on = False


@app.route('/api/entry_exit', methods=['POST'])
def entry_exit():
    """Endpoint to handle entry or exit based on name."""
    name = request.json.get("name")

    if name in valid_users:
        # If the member is valid, check if they're already in the gym
        if name not in people_in_gym:
            # Member enters the gym
            people_in_gym.add(name)
            return jsonify({"status": "success", "message": "Access granted, member entered", "fan": fan_on}), 200
        else:
            # Member exits the gym
            people_in_gym.remove(name)
            return jsonify({"status": "success", "message": "Member exited", "fan": fan_on}), 200
    else:
        # Invalid member
        return jsonify({"status": "error", "message": "Access denied, invalid membership"}), 200


@app.route('/api/people_count', methods=['GET'])
def get_people_count():
    """Endpoint to get the number of people currently inside."""
    return jsonify({"people_count": len(people_in_gym)}), 200


@app.route('/api/control_fan', methods=['POST'])
def control_fan():
    """Endpoint to control the fan based on the number of people."""
    global fan_on

    # Control logic for the fan
    if len(people_in_gym) > 5:  # Example condition to turn the fan on
        fan_on = True
    else:
        fan_on = False

    return jsonify({"status": "success", "fan_on": fan_on}), 200


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8000)
