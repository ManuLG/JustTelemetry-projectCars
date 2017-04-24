// Used for memory-mapped functionality
#include <windows.h>
#include "sharedmemory.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#using < System.dll >
using namespace System;
using namespace System::IO::Ports;
using namespace std;

#pragma warning(disable:4996)

// Name of the pCars memory mapped file
#define MAP_OBJECT_NAME "$pcars$"

int main()
{
	// Open the memory-mapped file
	HANDLE fileHandle = OpenFileMapping( PAGE_READONLY, FALSE, MAP_OBJECT_NAME );
	if (fileHandle == NULL)
	{
		printf( "Could not open file mapping object (%d).\n", GetLastError() );
		return 1;
	}

	// Get the data structure
	const SharedMemory* sharedData = (SharedMemory*)MapViewOfFile( fileHandle, PAGE_READONLY, 0, 0, sizeof(SharedMemory) );
	if (sharedData == NULL)
	{
		printf( "Could not map view of file (%d).\n", GetLastError() );

		CloseHandle( fileHandle );
		return 1;
	}

	// Ensure we're sync'd to the correct data version
	if ( sharedData->mVersion != SHARED_MEMORY_VERSION )
	{
		printf( "Data version mismatch\n");
		return 1;
	}

	// arduino settings
	//SerialPort^ arduino;
	//arduino = gcnew SerialPort("COM8", 9600);
	//arduino->Open();

	ofstream myfile;
	myfile.open("fichero.json");
	myfile << "[\n";

	while (true)
	{
		const bool isValidParticipantIndex = sharedData->mViewedParticipantIndex != -1 && sharedData->mViewedParticipantIndex < sharedData->mNumParticipants && sharedData->mViewedParticipantIndex < STORED_PARTICIPANTS_MAX;
		if ( isValidParticipantIndex )
		{
			const ParticipantInfo& viewedParticipantInfo = sharedData->mParticipantInfo[sharedData->mViewedParticipantIndex];
			//printf( "mParticipantName: (%s)\n", viewedParticipantInfo.mName );
			//printf( "mWorldPosition: (%f,%f,%f)\n", viewedParticipantInfo.mWorldPosition[0], viewedParticipantInfo.mWorldPosition[1], viewedParticipantInfo.mWorldPosition[2] );

			printf("%f\n", sharedData->mOdometerKM);
			// Posici�n en la pista
			float x = viewedParticipantInfo.mWorldPosition[0] * (-1); //Para invertir
			float y = viewedParticipantInfo.mWorldPosition[2];

			// Velocidad ruedas
			float di_rpm = sharedData->mTyreRPS[0] * (-1);
			float dd_rpm = sharedData->mTyreRPS[1] * (-1);
			float ti_rpm = sharedData->mTyreRPS[2] * (-1);
			float td_rpm = sharedData->mTyreRPS[3] * (-1);

			// �ngulo de la direcci�n
			float direccion = sharedData->mSteering;
			direccion = direccion * 90;

			// Porcentaje de aceleraci�n
			float throttle = sharedData->mThrottle;
			throttle *= 100;

			// Fuerzas G
			float fuerza_g_x = sharedData->mLocalAcceleration[0]; // Positivo hacia delante, negativo hacia atr�s
			float fuerza_g_y = sharedData->mLocalAcceleration[2]; // Positivo hacia la derecha, negativo hacia atr�s

			// Tiempo
			float tiempo = sharedData->mCurrentTime;
			tiempo *= 1000;

			float km = sharedData->mOdometerKM;
			//printf("Direcci�n: %f  --  Acelerador: %f\n", direccion, throttle);
			//printf("Eje X: %f  --  Eje Y: %f\n", fuerza_g_x, fuerza_g_y);
			//printf("Tyre height DI: %f -- Tyre Y DI: %f\n", sharedData->mTyreSlipSpeed[0], sharedData->mTyreSlipSpeed[2]);

			// [X, Y, di_rpm, dd_rpm, ti_rpm, td_rpm, direccion, throttle, fuerza_g_x, fuerza_g_y, tiempo]
			myfile << "[" << x << "," << y << ","; // Posicion
			myfile << di_rpm << "," << dd_rpm << "," << ti_rpm << "," << td_rpm << ","; // Velocidad ruedas
			myfile << direccion << "," << throttle << ","; // Direccion y aceleracion
			myfile << fuerza_g_x << "," << fuerza_g_y << ","; // Fuerzas g
			myfile << tiempo << "," << km << "],\n"; // Tiempo en milisegundos

			Sleep(100);
		}
		//printf( "mGameState: (%d)\n", sharedData->mGameState );
		//printf( "mSessionState: (%d)\n", sharedData->mSessionState );
		//printf( "mRaceState: (%d)\n", sharedData->mRaceState );
		//printf("maxRPM: (%f)\n", sharedData->mMaxRPM);
		//printf("mRPM: (%f)\n", sharedData->mRpm);
		//system("cls");

		/*sprintf(buf, "rpm%.2f;", sharedData->mRpm);
		String^ send = gcnew String(buf);
		arduino->WriteLine(send);		

		memset(buf, 0, sizeof buf);
		sprintf(buf, "maxrpm%.2f;", sharedData->mMaxRPM);
		send = gcnew String(buf);
		arduino->WriteLine(send);

		memset(buf, 0, sizeof buf);
		sprintf(buf, "gear%d;", sharedData->mGear);
		send = gcnew String(buf);
		arduino->WriteLine(send); */
	}

	// Cleanup
	//myfile << "]\n";
	myfile.close();
	UnmapViewOfFile( sharedData );
	CloseHandle( fileHandle );
	//arduino->Close();

	return 0;
}
