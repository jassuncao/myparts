INSERT INTO datatype (id, name) VALUES
(1, 'integer'),
(2, 'decimal'),
(3, 'text');

INSERT INTO condition (value, description, defaultCondition) VALUES
('New', 'Brand New', 1),
('Used', 'Used', 0),
('Damaged', 'Damaged', 0);

INSERT INTO part_unit (name, abbreviation, defaultUnit) VALUES
('Pieces', 'pcs', 1),
('Centimeter', 'cm', 0),
('Meters', 'm', 0);

INSERT INTO category (name, description, lft, rgt) VALUES
('Root Category', 'Root of all categories', 1, 20),
  ('Active Components', NULL, 2, 17),
    ('Semiconductors', NULL, 3, 12),
      ('Transistors', NULL, 4, 9),
        ('NPN', NULL, 5, 6),
        ('PNP', NULL, 7, 8),
      ('Integrated Circuits', NULL, 10, 11),
    ('Optoelectronics', NULL, 13, 14),
    ('Others', NULL, 15, 16),
  ('Passives', NULL, 18, 19);

INSERT INTO storage (name, description, lft, rgt) VALUES
('Root Storage', 'Root of all storage', 1, 16),
  ('Cabinet 1', NULL, 2, 15),
    ('Drawer 1', NULL, 3, 8),
        ('Compartment 1', NULL, 4, 5),
        ('Compartment 2', NULL, 6, 7),
    ('Drawer 1', NULL, 9, 14),
        ('Compartment 1', NULL, 10, 11),
        ('Compartment 2', NULL, 12, 13);


INSERT INTO unit (id, name, symbol, deletable) VALUES
(1,'Volts','V',0),
(2,'Amperes','A',0),
(3,'Ohms','Ω',0),
(4,'Farads','F',0),
(5,'Watts','W',0),
(6,'Hertz','Hz',0),
(7,'Henry','H',0),
(8,'Second','s',0),
(9,'Meter','m',0),
(10,'Decibel','dB',0),
(11,'Parts-per-million','ppm',0),
(12,'Candela','cd',0),
(13,'Celsius','°C',0),
(14,'American wire gauge','AWG',0),
(15,'Bit','b',0),
(16,'Byte','B',0);

INSERT INTO packaging (name) VALUES
('Single'),
('Tape & Reel'),
('Cut Tape'),
('Tube'),
('Digi-Reel®');


INSERT INTO parameter (key, name, datatype, deletable, unit, description) VALUES
('resistance', 'Resistance', 2, 0, 3, 'Resistance'),
('capacitance', 'Capacitance', 2, 0, 4, 'Capacitance'),
('power_rating', 'Power Rating', 2, 0, 5, 'Power Rating'),
('voltage_rating_dc', 'Voltage Rating (DC)', 2, 0, 1, 'Voltage Rating (DC)'),
('resistance_tolerance', 'Resistance Tolerance', 3, 0, NULL, 'Resistance Tolerance'),
('capacitance_tolerance', 'Capacitance Tolerance', 3, 0, NULL, 'Capacitance Tolerance');
