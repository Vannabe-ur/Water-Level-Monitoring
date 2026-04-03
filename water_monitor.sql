CREATE DATABASE water_monitor;

USE water_monitor;
CREATE TABLE water_level (
	id INT AUTO_INCREMENT PRIMARY KEY,
    water_cm FLOAT,
    status VARCHAR(20),
    create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


SELECT * FROM water_level;