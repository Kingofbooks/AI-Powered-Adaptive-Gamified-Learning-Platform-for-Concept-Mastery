#!/usr/bin/env python
"""Test registration endpoint"""

import requests
import json

url = "http://localhost:8000/auth/register"
payload = {
    "name": "John Doe",
    "email": "john@example.com",
    "username": "johndoe",
    "password": "password123",
    "role": "student",
    "college": "MIT",
    "department": "Computer Science",
    "year": "2nd"
}

print("=" * 60)
print("Testing Registration Endpoint")
print("=" * 60)
print(f"\nURL: {url}")
print(f"\nPayload:")
print(json.dumps(payload, indent=2))
print("\n" + "=" * 60)
print("RESPONSE:")
print("=" * 60)

try:
    response = requests.post(url, json=payload)
    print(f"Status Code: {response.status_code}")
    print(f"\nResponse Body:")
    print(json.dumps(response.json(), indent=2))
    
    if response.status_code == 201 or response.status_code == 200:
        print("\n✓ Registration successful!")
    else:
        print(f"\n✗ Registration failed with status {response.status_code}")
except Exception as e:
    print(f"Error: {e}")
