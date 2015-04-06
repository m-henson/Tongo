
using UnityEngine;
using System.Collections;
using System.IO.Ports;

public class ArduinoController : MonoBehaviour {
	
	SerialPort stream = new SerialPort("COM9", 9600); //Set the port (com4) and the baud rate (9600, is standard on most devices)
	int speed;
	float speed_scaler = 0.003f;
	float rotate_scaler = 0.1f;
	int direction;
	int mags = 16;
	int smooth = 2;
	Vector3 t_position;
	Vector3 t_rotation;
	float rotation;
	int i;
	string value;
	string[] vec;
	int encoded;
	public int arduino_rate = 100;

	
	void Start () {
		stream.Open(); //Open the Serial Stream.
		speed = 0;
		direction = 0;
		t_position = transform.position;
		t_rotation.x = 0.0f;
		t_rotation.z = 0.0f;
		i = 0;
		rotation = 0.0f;
	}
	
	// Update is called once per frame
	void Update () {
		i += 1;
		t_rotation.y = 0.0f;
		if (i >= arduino_rate)
		{
			i = 0;
			value = stream.ReadLine(); //Read the information
			if (value != "")
			{
				vec = value.Split(':');
				encoded = int.Parse(vec[0]);
				speed = encoded % mags;
				direction = (encoded - speed) / mags;
				rotation = transform.eulerAngles.y / 180.0f * Mathf.PI;
			}
		}
		if (direction == 0)
		{
			t_position.x -= speed * speed_scaler * Mathf.Cos(rotation);
			t_position.z += speed * speed_scaler * Mathf.Sin(rotation);
		}
		else if (direction == 2)
		{
			t_rotation.y = -speed * rotate_scaler;
		}
		else if (direction == 1)
		{
			t_rotation.y = speed * rotate_scaler;
		}
		transform.Rotate(t_rotation);
		transform.position = t_position;
	}
}
