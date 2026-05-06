-- Laboratorio 02

-- 01. Importar la fuente de datos proporcionada (hospital.sql) en el SGBD seleccionado.
DROP TABLE IF EXISTS Physician CASCADE;
CREATE TABLE Physician (
  EmployeeID INTEGER NOT NULL,
  Name VARCHAR(30) NOT NULL,
  Position VARCHAR(30) NOT NULL,
  SSN INTEGER NOT NULL,
  CONSTRAINT pk_physician PRIMARY KEY(EmployeeID)
); 

DROP TABLE IF EXISTS Department CASCADE;
CREATE TABLE Department (
  DepartmentID INTEGER NOT NULL,
  Name VARCHAR(30) NOT NULL,
  Head INTEGER NOT NULL,
  CONSTRAINT pk_Department PRIMARY KEY(DepartmentID),
  CONSTRAINT fk_Department_Physician_EmployeeID FOREIGN KEY(Head) REFERENCES Physician(EmployeeID)
);


DROP TABLE IF EXISTS Affiliated_With CASCADE;
CREATE TABLE Affiliated_With (
  Physician INTEGER NOT NULL,
  Department INTEGER NOT NULL,
  PrimaryAffiliation BOOLEAN NOT NULL,
  CONSTRAINT fk_Affiliated_With_Physician_EmployeeID FOREIGN KEY(Physician) REFERENCES Physician(EmployeeID),
  CONSTRAINT fk_Affiliated_With_Department_DepartmentID FOREIGN KEY(Department) REFERENCES Department(DepartmentID),
  PRIMARY KEY(Physician, Department)
);

DROP TABLE IF EXISTS Procedures CASCADE;
CREATE TABLE Procedures (
  Code INTEGER PRIMARY KEY NOT NULL,
  Name VARCHAR(30) NOT NULL,
  Cost REAL NOT NULL
);

DROP TABLE IF EXISTS Trained_In CASCADE;
CREATE TABLE Trained_In (
  Physician INTEGER NOT NULL,
  Treatment INTEGER NOT NULL,
  CertificationDate TIMESTAMP NOT NULL,
  CertificationExpires TIMESTAMP NOT NULL,
  CONSTRAINT fk_Trained_In_Physician_EmployeeID FOREIGN KEY(Physician) REFERENCES Physician(EmployeeID),
  CONSTRAINT fk_Trained_In_Procedures_Code FOREIGN KEY(Treatment) REFERENCES Procedures(Code),
  PRIMARY KEY(Physician, Treatment)
);

DROP TABLE IF EXISTS Patient CASCADE;
CREATE TABLE Patient (
  SSN INTEGER PRIMARY KEY NOT NULL,
  Name VARCHAR(30) NOT NULL,
  Address VARCHAR(30) NOT NULL,
  Phone VARCHAR(30) NOT NULL,
  InsuranceID INTEGER NOT NULL,
  PCP INTEGER NOT NULL,
  CONSTRAINT fk_Patient_Physician_EmployeeID FOREIGN KEY(PCP) REFERENCES Physician(EmployeeID)
);

DROP TABLE IF EXISTS Nurse CASCADE;
CREATE TABLE Nurse (
  EmployeeID INTEGER PRIMARY KEY NOT NULL,
  Name VARCHAR(30) NOT NULL,
  Position VARCHAR(30) NOT NULL,
  Registered BOOLEAN NOT NULL,
  SSN INTEGER NOT NULL
);

DROP TABLE IF EXISTS Appointment CASCADE;
CREATE TABLE Appointment (
  AppointmentID INTEGER PRIMARY KEY NOT NULL,
  Patient INTEGER NOT NULL,    
  PrepNurse INTEGER,
  Physician INTEGER NOT NULL,
  StartDate TIMESTAMP NOT NULL,
  EndDate TIMESTAMP NOT NULL,
  ExaminationRoom TEXT NOT NULL,
  CONSTRAINT fk_Appointment_Patient_SSN FOREIGN KEY(Patient) REFERENCES Patient(SSN),
  CONSTRAINT fk_Appointment_Nurse_EmployeeID FOREIGN KEY(PrepNurse) REFERENCES Nurse(EmployeeID),
  CONSTRAINT fk_Appointment_Physician_EmployeeID FOREIGN KEY(Physician) REFERENCES Physician(EmployeeID)
);

DROP TABLE IF EXISTS Medication CASCADE;
CREATE TABLE Medication (
  Code INTEGER PRIMARY KEY NOT NULL,
  Name VARCHAR(30) NOT NULL,
  Brand VARCHAR(30) NOT NULL,
  Description VARCHAR(30) NOT NULL
);


DROP TABLE IF EXISTS Prescribes CASCADE;
CREATE TABLE Prescribes (
  Physician INTEGER NOT NULL,
  Patient INTEGER NOT NULL, 
  Medication INTEGER NOT NULL, 
  Date TIMESTAMP NOT NULL,
  Appointment INTEGER,  
  Dose VARCHAR(30) NOT NULL,
  PRIMARY KEY(Physician, Patient, Medication, Date),
  CONSTRAINT fk_Prescribes_Physician_EmployeeID FOREIGN KEY(Physician) REFERENCES Physician(EmployeeID),
  CONSTRAINT fk_Prescribes_Patient_SSN FOREIGN KEY(Patient) REFERENCES Patient(SSN),
  CONSTRAINT fk_Prescribes_Medication_Code FOREIGN KEY(Medication) REFERENCES Medication(Code),
  CONSTRAINT fk_Prescribes_Appointment_AppointmentID FOREIGN KEY(Appointment) REFERENCES Appointment(AppointmentID)
);

DROP TABLE IF EXISTS Block CASCADE;
CREATE TABLE Block (
  BlockFloor INTEGER NOT NULL,
  BlockCode INTEGER NOT NULL,
  PRIMARY KEY(BlockFloor, BlockCode)
); 

DROP TABLE IF EXISTS Room CASCADE;
CREATE TABLE Room (
  RoomNumber INTEGER PRIMARY KEY NOT NULL,
  RoomType VARCHAR(30) NOT NULL,
  BlockFloor INTEGER NOT NULL,  
  BlockCode INTEGER NOT NULL,  
  Unavailable BOOLEAN NOT NULL,
  CONSTRAINT fk_Room_Block_PK FOREIGN KEY(BlockFloor, BlockCode) REFERENCES Block(BlockFloor, BlockCode)
);

DROP TABLE IF EXISTS On_Call CASCADE;
CREATE TABLE On_Call (
  Nurse INTEGER NOT NULL,
  BlockFloor INTEGER NOT NULL, 
  BlockCode INTEGER NOT NULL,
  OnCallStart TIMESTAMP NOT NULL,
  OnCallEnd TIMESTAMP NOT NULL,
  PRIMARY KEY(Nurse, BlockFloor, BlockCode, OnCallStart, OnCallEnd),
  CONSTRAINT fk_OnCall_Nurse_EmployeeID FOREIGN KEY(Nurse) REFERENCES Nurse(EmployeeID),
  CONSTRAINT fk_OnCall_Block_Floor FOREIGN KEY(BlockFloor, BlockCode) REFERENCES Block(BlockFloor, BlockCode) 
);

DROP TABLE IF EXISTS Stay CASCADE;
CREATE TABLE Stay (
  StayID INTEGER PRIMARY KEY NOT NULL,
  Patient INTEGER NOT NULL,
  Room INTEGER NOT NULL,
  StayStart TIMESTAMP NOT NULL,
  StayEnd TIMESTAMP NOT NULL,
  CONSTRAINT fk_Stay_Patient_SSN FOREIGN KEY(Patient) REFERENCES Patient(SSN),
  CONSTRAINT fk_Stay_Room_Number FOREIGN KEY(Room) REFERENCES Room(RoomNumber)
);

DROP TABLE IF EXISTS Undergoes CASCADE;
CREATE TABLE Undergoes (
  Patient INTEGER NOT NULL,
  Treatment INTEGER NOT NULL,
  Stay INTEGER NOT NULL,
  DateUndergoes TIMESTAMP NOT NULL,
  Physician INTEGER NOT NULL,
  AssistingNurse INTEGER,
  PRIMARY KEY(Patient, Treatment, Stay, DateUndergoes),
  CONSTRAINT fk_Undergoes_Patient_SSN FOREIGN KEY(Patient) REFERENCES Patient(SSN),
  CONSTRAINT fk_Undergoes_Procedures_Code FOREIGN KEY(Treatment) REFERENCES Procedures(Code),
  CONSTRAINT fk_Undergoes_Stay_StayID FOREIGN KEY(Stay) REFERENCES Stay(StayID),
  CONSTRAINT fk_Undergoes_Physician_EmployeeID FOREIGN KEY(Physician) REFERENCES Physician(EmployeeID),
  CONSTRAINT fk_Undergoes_Nurse_EmployeeID FOREIGN KEY(AssistingNurse) REFERENCES Nurse(EmployeeID)
);

INSERT INTO Physician VALUES(1,'John Dorian','Staff Internist',111111111);
INSERT INTO Physician VALUES(2,'Elliot Reid','Attending Physician',222222222);
INSERT INTO Physician VALUES(3,'Christopher Turk','Surgical Attending Physician',333333333);
INSERT INTO Physician VALUES(4,'Percival Cox','Senior Attending Physician',444444444);
INSERT INTO Physician VALUES(5,'Bob Kelso','Head Chief of Medicine',555555555);
INSERT INTO Physician VALUES(6,'Todd Quinlan','Surgical Attending Physician',666666666);
INSERT INTO Physician VALUES(7,'John Wen','Surgical Attending Physician',777777777);
INSERT INTO Physician VALUES(8,'Keith Dudemeister','MD Resident',888888888);
INSERT INTO Physician VALUES(9,'Molly Clock','Attending Psychiatrist',999999999);

INSERT INTO Department VALUES(1,'General Medicine',4);
INSERT INTO Department VALUES(2,'Surgery',7);
INSERT INTO Department VALUES(3,'Psychiatry',9);

INSERT INTO Affiliated_With VALUES(1,1,TRUE);
INSERT INTO Affiliated_With VALUES(2,1,TRUE);
INSERT INTO Affiliated_With VALUES(3,1,FALSE);
INSERT INTO Affiliated_With VALUES(3,2,TRUE);
INSERT INTO Affiliated_With VALUES(4,1,TRUE);
INSERT INTO Affiliated_With VALUES(5,1,TRUE);
INSERT INTO Affiliated_With VALUES(6,2,TRUE);
INSERT INTO Affiliated_With VALUES(7,1,FALSE);
INSERT INTO Affiliated_With VALUES(7,2,TRUE);
INSERT INTO Affiliated_With VALUES(8,1,TRUE);
INSERT INTO Affiliated_With VALUES(9,3,TRUE);

INSERT INTO Procedures VALUES(1,'Reverse Rhinopodoplasty',1500.0);
INSERT INTO Procedures VALUES(2,'Obtuse Pyloric Recombobulation',3750.0);
INSERT INTO Procedures VALUES(3,'Folded Demiophtalmectomy',4500.0);
INSERT INTO Procedures VALUES(4,'Complete Walletectomy',10000.0);
INSERT INTO Procedures VALUES(5,'Obfuscated Dermogastrotomy',4899.0);
INSERT INTO Procedures VALUES(6,'Reversible Pancreomyoplasty',5600.0);
INSERT INTO Procedures VALUES(7,'Follicular Demiectomy',25.0);

INSERT INTO Patient VALUES(100000001,'John Smith','42 Foobar Lane','555-0256',68476213,1);
INSERT INTO Patient VALUES(100000002,'Grace Ritchie','37 Snafu Drive','555-0512',36546321,2);
INSERT INTO Patient VALUES(100000003,'Random J. Patient','101 Omgbbq Street','555-1204',65465421,2);
INSERT INTO Patient VALUES(100000004,'Dennis Doe','1100 Foobaz Avenue','555-2048',68421879,3);

INSERT INTO Nurse VALUES(101,'Carla Espinosa','Head Nurse',TRUE,111111110);
INSERT INTO Nurse VALUES(102,'Laverne Roberts','Nurse',TRUE,222222220);
INSERT INTO Nurse VALUES(103,'Paul Flowers','Nurse',FALSE,333333330);

INSERT INTO Appointment VALUES(13216584,100000001,101,1,'2008-04-24 10:00','2008-04-24 11:00','A');
INSERT INTO Appointment VALUES(26548913,100000002,101,2,'2008-04-24 10:00','2008-04-24 11:00','B');
INSERT INTO Appointment VALUES(36549879,100000001,102,1,'2008-04-25 10:00','2008-04-25 11:00','A');
INSERT INTO Appointment VALUES(46846589,100000004,103,4,'2008-04-25 10:00','2008-04-25 11:00','B');
INSERT INTO Appointment VALUES(59871321,100000004,NULL,4,'2008-04-26 10:00','2008-04-26 11:00','C');
INSERT INTO Appointment VALUES(69879231,100000003,103,2,'2008-04-26 11:00','2008-04-26 12:00','C');
INSERT INTO Appointment VALUES(76983231,100000001,NULL,3,'2008-04-26 12:00','2008-04-26 13:00','C');
INSERT INTO Appointment VALUES(86213939,100000004,102,9,'2008-04-27 10:00','2008-04-21 11:00','A');
INSERT INTO Appointment VALUES(93216548,100000002,101,2,'2008-04-27 10:00','2008-04-27 11:00','B');

INSERT INTO Medication VALUES(1,'Procrastin-X','X','N/A');
INSERT INTO Medication VALUES(2,'Thesisin','Foo Labs','N/A');
INSERT INTO Medication VALUES(3,'Awakin','Bar Laboratories','N/A');
INSERT INTO Medication VALUES(4,'Crescavitin','Baz Industries','N/A');
INSERT INTO Medication VALUES(5,'Melioraurin','Snafu Pharmaceuticals','N/A');

INSERT INTO Prescribes VALUES(1,100000001,1,'2008-04-24 10:47',13216584,'5');
INSERT INTO Prescribes VALUES(9,100000004,2,'2008-04-27 10:53',86213939,'10');
INSERT INTO Prescribes VALUES(9,100000004,2,'2008-04-30 16:53',NULL,'5');

INSERT INTO Block VALUES(1,1);
INSERT INTO Block VALUES(1,2);
INSERT INTO Block VALUES(1,3);
INSERT INTO Block VALUES(2,1);
INSERT INTO Block VALUES(2,2);
INSERT INTO Block VALUES(2,3);
INSERT INTO Block VALUES(3,1);
INSERT INTO Block VALUES(3,2);
INSERT INTO Block VALUES(3,3);
INSERT INTO Block VALUES(4,1);
INSERT INTO Block VALUES(4,2);
INSERT INTO Block VALUES(4,3);

INSERT INTO Room VALUES(101,'Single',1,1,FALSE);
INSERT INTO Room VALUES(102,'Single',1,1,FALSE);
INSERT INTO Room VALUES(103,'Single',1,1,FALSE);
INSERT INTO Room VALUES(111,'Single',1,2,FALSE);
INSERT INTO Room VALUES(112,'Single',1,2,TRUE);
INSERT INTO Room VALUES(113,'Single',1,2,FALSE);
INSERT INTO Room VALUES(121,'Single',1,3,FALSE);
INSERT INTO Room VALUES(122,'Single',1,3,FALSE);
INSERT INTO Room VALUES(123,'Single',1,3,FALSE);
INSERT INTO Room VALUES(201,'Single',2,1,TRUE);
INSERT INTO Room VALUES(202,'Single',2,1,FALSE);
INSERT INTO Room VALUES(203,'Single',2,1,FALSE);
INSERT INTO Room VALUES(211,'Single',2,2,FALSE);
INSERT INTO Room VALUES(212,'Single',2,2,FALSE);
INSERT INTO Room VALUES(213,'Single',2,2,TRUE);
INSERT INTO Room VALUES(221,'Single',2,3,FALSE);
INSERT INTO Room VALUES(222,'Single',2,3,FALSE);
INSERT INTO Room VALUES(223,'Single',2,3,FALSE);
INSERT INTO Room VALUES(301,'Single',3,1,FALSE);
INSERT INTO Room VALUES(302,'Single',3,1,TRUE);
INSERT INTO Room VALUES(303,'Single',3,1,FALSE);
INSERT INTO Room VALUES(311,'Single',3,2,FALSE);
INSERT INTO Room VALUES(312,'Single',3,2,FALSE);
INSERT INTO Room VALUES(313,'Single',3,2,FALSE);
INSERT INTO Room VALUES(321,'Single',3,3,TRUE);
INSERT INTO Room VALUES(322,'Single',3,3,FALSE);
INSERT INTO Room VALUES(323,'Single',3,3,FALSE);
INSERT INTO Room VALUES(401,'Single',4,1,FALSE);
INSERT INTO Room VALUES(402,'Single',4,1,TRUE);
INSERT INTO Room VALUES(403,'Single',4,1,FALSE);
INSERT INTO Room VALUES(411,'Single',4,2,FALSE);
INSERT INTO Room VALUES(412,'Single',4,2,FALSE);
INSERT INTO Room VALUES(413,'Single',4,2,FALSE);
INSERT INTO Room VALUES(421,'Single',4,3,TRUE);
INSERT INTO Room VALUES(422,'Single',4,3,FALSE);
INSERT INTO Room VALUES(423,'Single',4,3,FALSE);

INSERT INTO On_Call VALUES(101,1,1,'2008-11-04 11:00','2008-11-04 19:00');
INSERT INTO On_Call VALUES(101,1,2,'2008-11-04 11:00','2008-11-04 19:00');
INSERT INTO On_Call VALUES(102,1,3,'2008-11-04 11:00','2008-11-04 19:00');
INSERT INTO On_Call VALUES(103,1,1,'2008-11-04 19:00','2008-11-05 03:00');
INSERT INTO On_Call VALUES(103,1,2,'2008-11-04 19:00','2008-11-05 03:00');
INSERT INTO On_Call VALUES(103,1,3,'2008-11-04 19:00','2008-11-05 03:00');

INSERT INTO Stay VALUES(3215,100000001,111,'2008-05-01','2008-05-04');
INSERT INTO Stay VALUES(3216,100000003,123,'2008-05-03','2008-05-14');
INSERT INTO Stay VALUES(3217,100000004,112,'2008-05-02','2008-05-03');

INSERT INTO Undergoes VALUES(100000001,6,3215,'2008-05-02',3,101);
INSERT INTO Undergoes VALUES(100000001,2,3215,'2008-05-03',7,101);
INSERT INTO Undergoes VALUES(100000004,1,3217,'2008-05-07',3,102);
INSERT INTO Undergoes VALUES(100000004,5,3217,'2008-05-09',6,NULL);
INSERT INTO Undergoes VALUES(100000001,7,3217,'2008-05-10',7,101);
INSERT INTO Undergoes VALUES(100000004,4,3217,'2008-05-13',3,103);

INSERT INTO Trained_In VALUES(3,1,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(3,2,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(3,5,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(3,6,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(3,7,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(6,2,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(6,5,'2007-01-01','2007-12-31');
INSERT INTO Trained_In VALUES(6,6,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(7,1,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(7,2,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(7,3,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(7,4,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(7,5,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(7,6,'2008-01-01','2008-12-31');
INSERT INTO Trained_In VALUES(7,7,'2008-01-01','2008-12-31');

-- 2. Diseñar un cubo OLAP de 4 dimensiones
--    Dimensiones: medico, paciente, tiempo y procedimiento
--    Hechos: costo_tratamiento, duracion_dias

-- 3. Implementar el cubo OLAP en PostgreSQL (esquema estrella)
-- 3.b + 3.c: DDL de creacion del esquema

DROP VIEW IF EXISTS dw_cubo_tratamientos_4d CASCADE;
DROP TABLE IF EXISTS dw_fact_tratamiento CASCADE;
DROP TABLE IF EXISTS dw_dim_tiempo CASCADE;
DROP TABLE IF EXISTS dw_dim_procedimiento CASCADE;
DROP TABLE IF EXISTS dw_dim_paciente CASCADE;
DROP TABLE IF EXISTS dw_dim_medico CASCADE;

CREATE TABLE dw_dim_medico (
  medico_id INTEGER PRIMARY KEY,
  nombre VARCHAR(30) NOT NULL,
  especialidad VARCHAR(255) NOT NULL,
  ssn INTEGER NOT NULL
);

CREATE TABLE dw_dim_paciente (
  paciente_id INTEGER PRIMARY KEY,
  nombre VARCHAR(30) NOT NULL,
  direccion VARCHAR(30) NOT NULL,
  telefono VARCHAR(30) NOT NULL,
  seguro INTEGER NOT NULL
);

CREATE TABLE dw_dim_procedimiento (
  procedimiento_id INTEGER PRIMARY KEY,
  nombre VARCHAR(30) NOT NULL,
  costo_base NUMERIC(12,2) NOT NULL
);

CREATE TABLE dw_dim_tiempo (
  tiempo_id TIMESTAMP PRIMARY KEY,
  fecha DATE NOT NULL,
  hora SMALLINT NOT NULL,
  minuto SMALLINT NOT NULL,
  dia SMALLINT NOT NULL,
  semana SMALLINT NOT NULL,
  mes SMALLINT NOT NULL,
  anio SMALLINT NOT NULL
);

CREATE TABLE dw_fact_tratamiento (
  fact_id BIGSERIAL PRIMARY KEY,
  medico_id INTEGER NOT NULL,
  paciente_id INTEGER NOT NULL,
  procedimiento_id INTEGER NOT NULL,
  tiempo_id TIMESTAMP NOT NULL,
  estancia_id INTEGER NOT NULL,
  costo_tratamiento NUMERIC(12,2) NOT NULL,
  duracion_dias NUMERIC(12,2) NOT NULL,
  CONSTRAINT fk_fact_medico FOREIGN KEY (medico_id) REFERENCES dw_dim_medico(medico_id),
  CONSTRAINT fk_fact_paciente FOREIGN KEY (paciente_id) REFERENCES dw_dim_paciente(paciente_id),
  CONSTRAINT fk_fact_procedimiento FOREIGN KEY (procedimiento_id) REFERENCES dw_dim_procedimiento(procedimiento_id),
  CONSTRAINT fk_fact_tiempo FOREIGN KEY (tiempo_id) REFERENCES dw_dim_tiempo(tiempo_id)
);

CREATE INDEX idx_fact_medico ON dw_fact_tratamiento(medico_id);
CREATE INDEX idx_fact_paciente ON dw_fact_tratamiento(paciente_id);
CREATE INDEX idx_fact_procedimiento ON dw_fact_tratamiento(procedimiento_id);
CREATE INDEX idx_fact_tiempo ON dw_fact_tratamiento(tiempo_id);

-- 3.e + 3.f: DML para poblar dimensiones y hechos
INSERT INTO dw_dim_medico (medico_id, nombre, especialidad, ssn)
SELECT
  p.EmployeeID,
  p.Name,
  COALESCE(
    string_agg(DISTINCT pr.Name, ', ' ORDER BY pr.Name),
    p.Position
  ) AS especialidad,
  p.SSN
FROM Physician p
LEFT JOIN Trained_In ti ON ti.Physician = p.EmployeeID
LEFT JOIN Procedures pr ON pr.Code = ti.Treatment
GROUP BY p.EmployeeID, p.Name, p.Position, p.SSN;

INSERT INTO dw_dim_paciente (paciente_id, nombre, direccion, telefono, seguro)
SELECT
  pa.SSN,
  pa.Name,
  pa.Address,
  pa.Phone,
  pa.InsuranceID
FROM Patient pa;

INSERT INTO dw_dim_procedimiento (procedimiento_id, nombre, costo_base)
SELECT
  pr.Code,
  pr.Name,
  pr.Cost::NUMERIC(12,2)
FROM Procedures pr;

INSERT INTO dw_dim_tiempo (tiempo_id, fecha, hora, minuto, dia, semana, mes, anio)
SELECT DISTINCT
  date_trunc('minute', u.DateUndergoes) AS tiempo_id,
  date_trunc('day', u.DateUndergoes)::DATE AS fecha,
  EXTRACT(HOUR FROM u.DateUndergoes)::SMALLINT AS hora,
  EXTRACT(MINUTE FROM u.DateUndergoes)::SMALLINT AS minuto,
  EXTRACT(DAY FROM u.DateUndergoes)::SMALLINT AS dia,
  EXTRACT(WEEK FROM u.DateUndergoes)::SMALLINT AS semana,
  EXTRACT(MONTH FROM u.DateUndergoes)::SMALLINT AS mes,
  EXTRACT(YEAR FROM u.DateUndergoes)::SMALLINT AS anio
FROM Undergoes u;

INSERT INTO dw_fact_tratamiento (
  medico_id,
  paciente_id,
  procedimiento_id,
  tiempo_id,
  estancia_id,
  costo_tratamiento,
  duracion_dias
)
SELECT
  u.Physician,
  u.Patient,
  u.Treatment,
  date_trunc('minute', u.DateUndergoes),
  u.Stay,
  pr.Cost::NUMERIC(12,2),
  ROUND(EXTRACT(EPOCH FROM (s.StayEnd - s.StayStart)) / 86400.0, 2)::NUMERIC(12,2)
FROM Undergoes u
JOIN Stay s ON s.StayID = u.Stay
JOIN Procedures pr ON pr.Code = u.Treatment;

CREATE OR REPLACE VIEW dw_cubo_tratamientos_4d AS
SELECT
  CASE
    WHEN GROUPING(m.medico_id) = 1 THEN 'TOTAL'
    ELSE m.nombre
  END AS medico,
  CASE
    WHEN GROUPING(p.paciente_id) = 1 THEN 'TOTAL'
    ELSE p.nombre
  END AS paciente,
  CASE
    WHEN GROUPING(t.tiempo_id) = 1 THEN 'TOTAL'
    ELSE to_char(t.tiempo_id, 'DD/MM/YYYY HH24:MI')
  END AS tiempo,
  CASE
    WHEN GROUPING(pr.procedimiento_id) = 1 THEN 'TOTAL'
    ELSE pr.nombre
  END AS procedimiento,
  SUM(f.costo_tratamiento) AS costo_total,
  SUM(f.duracion_dias) AS duracion_total_dias,
  COUNT(*) AS cantidad_tratamientos
FROM dw_fact_tratamiento f
JOIN dw_dim_medico m ON m.medico_id = f.medico_id
JOIN dw_dim_paciente p ON p.paciente_id = f.paciente_id
JOIN dw_dim_tiempo t ON t.tiempo_id = f.tiempo_id
JOIN dw_dim_procedimiento pr ON pr.procedimiento_id = f.procedimiento_id
GROUP BY CUBE (
  (m.medico_id, m.nombre),
  (p.paciente_id, p.nombre),
  (t.tiempo_id),
  (pr.procedimiento_id, pr.nombre)
);

-- 
-- 4. Consultas usando RANK() y DENSE_RANK()
-- a) Ordenar los tratamientos recibidos por cada paciente por costo (desc) y por duracion (desc)
--    Usamos primero RANK(), luego DENSE_RANK() para comparar resultados.

-- RANK() por costo (mayor a menor) — deja huecos si hay empates
-- Muestra la posición del tratamiento por paciente según el costo
-- Consulta 1: RANK() por costo y luego duracion (por paciente)
SELECT
  f.paciente_id,
  p.nombre AS paciente,
  pr.nombre AS procedimiento,
  f.costo_tratamiento,
  f.duracion_dias,
  RANK() OVER (
    PARTITION BY f.paciente_id
    ORDER BY f.costo_tratamiento DESC, f.duracion_dias DESC
  ) AS rank_costo_duracion
FROM dw_fact_tratamiento f
JOIN dw_dim_paciente p ON p.paciente_id = f.paciente_id
JOIN dw_dim_procedimiento pr ON pr.procedimiento_id = f.procedimiento_id
ORDER BY f.paciente_id, rank_costo_duracion, f.costo_tratamiento DESC, f.duracion_dias DESC;

-- Consulta 2: DENSE_RANK() por costo y luego duracion (por paciente)
SELECT
  f.paciente_id,
  p.nombre AS paciente,
  pr.nombre AS procedimiento,
  f.costo_tratamiento,
  f.duracion_dias,
  DENSE_RANK() OVER (
    PARTITION BY f.paciente_id
    ORDER BY f.costo_tratamiento DESC, f.duracion_dias DESC
  ) AS dense_rank_costo_duracion
FROM dw_fact_tratamiento f
JOIN dw_dim_paciente p ON p.paciente_id = f.paciente_id
JOIN dw_dim_procedimiento pr ON pr.procedimiento_id = f.procedimiento_id
ORDER BY f.paciente_id, dense_rank_costo_duracion, f.costo_tratamiento DESC, f.duracion_dias DESC;

-- Explicacion corta (para el entregable):
-- - RANK(): asigna el mismo rango a las filas empatadas pero deja huecos en la numeracion siguiente.
--   Ej: valores [100,100,90] -> RANKs [1,1,3].
-- - DENSE_RANK(): asigna el mismo rango a las filas empatadas pero la numeracion siguiente no tiene huecos.
--   Ej: valores [100,100,90] -> DENSE_RANKs [1,1,2].
-- Uso: elegir RANK si queres reflejar saltos en posiciones; elegir DENSE_RANK si preferis posiciones compactas.

-- 5. Consulta avanzada sobre el cubo OLAP
-- Obtener pacientes con tratamientos de un día de duración de la primera semana
-- de mayo de 2008, con análisis multidimensional usando CUBE, RANKING y SUB-QUERIES

-- Obtener el listado de pacientes con tratamientos de un día de duración de la
-- primera semana del mes de mayo de 2008, ordenar el listado por nombre de
-- paciente.

SELECT
FROM dw_fact_tratamiento f
JOIN dw_dim_paciente p ON p.paciente_id = f.paciente_id
JOIN dw_dim_procedimiento pr ON pr.procedimiento_id = f.procedimiento_id
JOIN dw_dim_tiempo t ON t.tiempo_id = f.tiempo_id

SELECT
  COALESCE(p.nombre, 'TOTAL GENERAL') AS paciente,
  COALESCE(pr.nombre, 'TOTAL PROCEDIMIENTO') AS procedimiento,
  COALESCE(t.fecha::TEXT, 'TOTAL FECHA') AS fecha,
  f.duracion_dias,
  COUNT(1) AS cantidad_tratamientos,
  SUM(f.costo_tratamiento) AS costo_total,
  AVG(f.costo_tratamiento) AS costo_promedio,
  RANK() OVER (
    PARTITION BY p.nombre
    ORDER BY SUM(f.costo_tratamiento) DESC
  ) AS rank_costo_por_paciente,
  DENSE_RANK() OVER (
    ORDER BY SUM(f.costo_tratamiento) DESC
  ) AS dense_rank_costo_global,
  GROUPING(p.nombre) AS grouping_paciente,
  GROUPING(pr.nombre) AS grouping_procedimiento,
  GROUPING(t.fecha) AS grouping_fecha
FROM dw_fact_tratamiento f
JOIN dw_dim_paciente p ON p.paciente_id = f.paciente_id
JOIN dw_dim_procedimiento pr ON pr.procedimiento_id = f.procedimiento_id
JOIN dw_dim_tiempo t ON t.tiempo_id = f.tiempo_id
WHERE
  -- SUB-QUERY: Filtrar solo pacientes con al menos un tratamiento de 1 día en mayo 2008 semana 1
  f.paciente_id IN (
    SELECT DISTINCT sub_f.paciente_id
    FROM dw_fact_tratamiento sub_f
    JOIN dw_dim_tiempo sub_t ON sub_t.tiempo_id = sub_f.tiempo_id
    WHERE sub_f.duracion_dias = 1
      AND sub_t.anio = 2008
      AND sub_t.mes = 5
      AND sub_t.dia BETWEEN 1 AND 7
  )
  -- WHERE: Filtros principales
  AND f.duracion_dias = 1
  AND t.anio = 2008
  AND t.mes = 5
  AND t.dia BETWEEN 1 AND 7
GROUP BY
  -- CUBE: Genera todas las combinaciones de dimensiones
  CUBE(p.nombre, pr.nombre, t.fecha, f.duracion_dias)
ORDER BY
  GROUPING(p.nombre) ASC,
  paciente ASC,
  GROUPING(pr.nombre) ASC,
  procedimiento ASC,
  fecha ASC;

