//g++ main.c `mysql_config --cflags` `mysql_config --libs`
#define SMART_MONITOR_VER 0.1
#define MAXPORTS 28
#define DEBUG TRUE
#include "conn.h"
#include "read_dht.c"

char str[8];
unsigned int availablePorts [MAXPORTS]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
										15,16,21,22,23,24,25,26,27,28,29,30,31};
										
unsigned int activePorts [MAXPORTS]={1,15,16};


int main (){
	
	if(wiringPiSetup () == 1)
		exit (1);
	resolvSensorID (0,1);
	readActivePorts ();
	/*displayDHTData (15);
	displayDHTData (1);*/

	
	/*if (!mysql_connect ()){
		test_insert (1,1,21.3);
		//test_update ();
		mysql_disconnect ();
	}*/
	return 0;
}

int readActivePorts (void){
	
	for  (int i=0 ; ((i < MAXPORTS) && (activePorts[i] != 0)) ; i++){
		//printf ("%d \n",activePorts[i]);
		displayDHTData (activePorts[i]);
	}

	return 0;
}
//  NOTE : If the prepare operation was unsuccessful 
// (that is, mysql_stmt_prepare() returns nonzero), 
// the error message can be obtained by calling mysql_stmt_error().
/* This function insert into DB the read data from sensor*/

int test_insert (short type, short id, float reading){

	short sql_error = 0;
	MYSQL_STMT *sql_statement1;
	MYSQL_BIND sql_bind_parameters1[3];
	

	//Setup to create query
	sql_statement1= mysql_stmt_init(mysql1);
	
	if (!sql_statement1){
		sql_error = 1;
		printf ("Error in %d fallo mysql_stmt_init(mysql1)\n",sql_error);
	}
	
	//----- SET THE QUERY TEXT -----
	#define SQL_QUERY_TEXT_1 "INSERT INTO \
	sensores_registros(id_sensor,registro,created_at) \
	VALUES(?,?,now())"
	
	if (mysql_stmt_prepare(sql_statement1, SQL_QUERY_TEXT_1, strlen(SQL_QUERY_TEXT_1))){
		sql_error = 2;
		printf("Error in %d fallo mysql_stmt_prepare(sql_statement1, SQL_QUERY_TEXT_1, strlen(SQL_QUERY_TEXT_1))\n",sql_error);
	}

	//----- SET THE QUERY PARAMETER VALUES -----
	//If you want to know how many parameters are expected
	//int param_count = mysql_stmt_param_count(sql_statement1);
	
	//Set the parameter values
	memset(sql_bind_parameters1, 0, sizeof(sql_bind_parameters1));		//Reset the parameters memory to null

	//Integer
	/*char str_data[8] = "TEMP01";
	
	unsigned long str_length = strlen(str_data);
	sql_bind_parameters1[0].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[0].buffer = (char*)str_data;
	sql_bind_parameters1[0].buffer_length = sizeof(str_data);
	sql_bind_parameters1[0].is_null = 0;
	sql_bind_parameters1[0].length = &str_length;			//<Note: this is a pointer!
	*/
	
	unsigned long str_length = strlen(str);
	sql_bind_parameters1[0].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[0].buffer = (char*)str;
	sql_bind_parameters1[0].buffer_length = sizeof(str);
	sql_bind_parameters1[0].is_null = 0;
	sql_bind_parameters1[0].length = &str_length;			//<Note: this is a pointer!
	//Integer
	/*int int_data1 = 20;
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_LONG;
	sql_bind_parameters1[1].buffer = (char*)&int_data1;		//<Note: this is a pointer!
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = 0;
	*/
	
	//Float
	//float int_data1 = 20.5;
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_FLOAT;
	sql_bind_parameters1[1].buffer = (char*)&reading;		//<Note: this is a pointer!
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = 0;

	
	//string
	/*char str_data[50] = "Hello";
	unsigned long str_length = strlen(str_data);
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[1].buffer = (char*)str_data;
	sql_bind_parameters1[1].buffer_length = sizeof(str_data);
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = &str_length;			//<Note: this is a pointer!
	*/
	//smallint
	/*short small_data;
	my_bool is_null = 1;		//We'll store this as null in this example
	sql_bind_parameters1[2].buffer_type = MYSQL_TYPE_SHORT;
	sql_bind_parameters1[2].buffer = (char*)&small_data;	//<Note: this is a pointer!
	sql_bind_parameters1[2].is_null = &is_null;				//<Note: this is a pointer!
	sql_bind_parameters1[2].length = 0;
	*/
	//Pointers are used in the bind parameters so that if you are say adding multiple rows you can use the same query setup with new values for each execute of it.
	
	//Bind the buffers
	if (mysql_stmt_bind_param(sql_statement1, sql_bind_parameters1)){
		sql_error = 3;
		printf("Error in %d fallo mysql_stmt_bind_param(sql_statement1, sql_bind_parameters1)\n",sql_error);
	}

	//----- EXECUTE THE QUERY ------
	if (!sql_error){
		if (mysql_stmt_execute(sql_statement1)){
			sql_error = 4;
			fprintf(stderr, "%s\n", sql_statement1);
			printf("Error in %d fallo. mysql_stmt_execute(sql_statement1)\n",sql_error);
		}
	}

	//If you want to get the number of affected rows
	long affected_rows = mysql_stmt_affected_rows(sql_statement1);
	//if (affected_rows != 1)
	//{
	//	do something
	//}
	printf ("Affected Rows %d \n", affected_rows); 
	//IF YOU WANT TO GET THE VALUE GENERATED FOR AN AUTO_INCREMENT COLUMN IN THE PREVIOUS INSERT/UPDATE STATEMENT
	//my_ulonglong sql_insert_id = mysql_stmt_insert_id(sql_statement1);

	//If you want to do the query again then change any values you want to change and call mysql_stmt_execute(sql_statement1) again

	//Close the statement
	if (sql_statement1){
		if (mysql_stmt_close(sql_statement1)){
			sql_error = 5;
			printf("Error in %d fallo.mysql_stmt_close(sql_statement1)\n",sql_error);
		}	
	}else
		printf ("Commit Succefull \n");
	
	return sql_error;
}

// Function to convert sensor reading to string

int resolvSensorReading (long reading){
	
	return 0;
}


int test_update (void){

	MYSQL_STMT *sql_statement1;
	MYSQL_BIND sql_bind_parameters1[2];
	bool sql_error = false;
	

	//Setup to create query
	sql_statement1= mysql_stmt_init(mysql1);
	
	if (!sql_statement1){
		sql_error = 1;
		printf("fallo 1.\n");
	}
	

	
	//----- SET THE QUERY TEXT -----
	#define SQL_QUERY_TEXT_2 "UPDATE \
	sensors SET id_error = ? WHERE id_error >= 10 and id = ?"
	// ErrorType SensorName
	
	if (mysql_stmt_prepare(sql_statement1, SQL_QUERY_TEXT_2, strlen(SQL_QUERY_TEXT_2))){
			sql_error = 1;
			printf("fallo 2.\n");
	}

	//----- SET THE QUERY PARAMETER VALUES -----
	//If you want to know how many parameters are expected
	//int param_count = mysql_stmt_param_count(sql_statement1);
	
	//Set the parameter values
	memset(sql_bind_parameters1, 0, sizeof(sql_bind_parameters1));		//Reset the parameters memory to null

	//Integer
	char str_data[8] = "TEMP01";
	unsigned long str_length = strlen(str_data);
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[1].buffer = (char*)str_data;
	sql_bind_parameters1[1].buffer_length = sizeof(str_data);
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = &str_length;			//<Note: this is a pointer!

	
	//Integer
	int int_data1 = 0;
	sql_bind_parameters1[0].buffer_type = MYSQL_TYPE_LONG;
	sql_bind_parameters1[0].buffer = (char*)&int_data1;		//<Note: this is a pointer!
	sql_bind_parameters1[0].is_null = 0;
	sql_bind_parameters1[0].length = 0;

	//string
	/*char str_data[50] = "Hello";
	unsigned long str_length = strlen(str_data);
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[1].buffer = (char*)str_data;
	sql_bind_parameters1[1].buffer_length = sizeof(str_data);
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = &str_length;			//<Note: this is a pointer!
	*/
	//smallint
	/*short small_data;
	my_bool is_null = 1;		//We'll store this as null in this example
	sql_bind_parameters1[2].buffer_type = MYSQL_TYPE_SHORT;
	sql_bind_parameters1[2].buffer = (char*)&small_data;	//<Note: this is a pointer!
	sql_bind_parameters1[2].is_null = &is_null;				//<Note: this is a pointer!
	sql_bind_parameters1[2].length = 0;
	*/
	//Pointers are used in the bind parameters so that if you are say adding multiple rows you can use the same query setup with new values for each execute of it.
	
	//Bind the buffers
	if (mysql_stmt_bind_param(sql_statement1, sql_bind_parameters1)){
		sql_error = 1;
		printf("fallo. 3\n");
	}

	//----- EXECUTE THE QUERY ------
	if (!sql_error)
	{
		if (mysql_stmt_execute(sql_statement1)){
			sql_error = 1;
			fprintf(stderr, "%s\n", sql_statement1);
			printf("fallo. 4\n");
		}
	}

	//If you want to get the number of affected rows
	//my_ulonglong affected_rows = mysql_stmt_affected_rows(sql_statement1);
	//if (affected_rows != 1)
	//{
	//	do something
	//}

	//IF YOU WANT TO GET THE VALUE GENERATED FOR AN AUTO_INCREMENT COLUMN IN THE PREVIOUS INSERT/UPDATE STATEMENT
	//my_ulonglong sql_insert_id = mysql_stmt_insert_id(sql_statement1);

	//If you want to do the query again then change any values you want to change and call mysql_stmt_execute(sql_statement1) again

	//Close the statement
	if (sql_statement1){
		if (mysql_stmt_close(sql_statement1)){
			sql_error = 1;
			printf("fallo. 5\n");
		}	
	}
	return 0;
	
}	
	
// Function to convert sensor name and id to string
	
int resolvSensorID (short type, short id){
		
	if ((type < 8) && (type >= 0) && (id < 99) && (id > 0)){
		strcat (str,sensorType[type]);
		sprintf (str+4, "%02d", id);
#ifdef DEBUG		
		printf ("%s \n",str);
#endif		
	}
	else{
#ifdef DEBUG		
		printf ("error %s \n",str);
#endif	
		return 1;
	}
	return 0;
}

// This function calculates the DewPoint of a given Hum & Temp

float calcDewPoint (float temp, float hum){
	
	float dewPoint; 
	
	dewPoint= temp-((100-hum)/5);
	return dewPoint;
}

int displayDHTData (unsigned short pin) {
	unsigned short counter = 0;
	
	while ((read_dht_data (pin)) && (counter < 250))
		counter++;
	
	if (counter >= 250){
		printf ("NOT DHT Sensor on pin %d \n", pin);
		
	}
	else{
		if (!DHTsensorType)
			printf ("Sensor is DHT22 ");
		else
			printf ("Sensor is DHT11 ");
		
		printf ("Temperatura: %2.1f Humedad: %2.1f Dew Point %2.1f\n", temperature, humidity,calcDewPoint (temperature, humidity));
	}
	return 0;
}

int versions (void){
	
	printf ("Version Smart Monitor %2.2f \n", SMART_MONITOR_VER );
	printf ("Version AN2302/DHT22 Sensor %2.2f \n", DHT_VER );
	return 0;
}