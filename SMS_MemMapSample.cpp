// Used for memory-mapped functionality
#include <windows.h>
#include "sharedmemory.h"
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#using < System.dll >
using namespace System;
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

	ofstream myfile;
	myfile.open("fichero.json");
	myfile << "[\n";

	myfile << "[\"" << sharedData->mTrackLocation << "\",\"" << sharedData->mCarName << "\",\"" << sharedData->mCarClassName << "\"],\n";

	while (true)
	{
		const bool isValidParticipantIndex = sharedData->mViewedParticipantIndex != -1 && sharedData->mViewedParticipantIndex < sharedData->mNumParticipants && sharedData->mViewedParticipantIndex < STORED_PARTICIPANTS_MAX;
		if ( isValidParticipantIndex )
		{
			const ParticipantInfo& viewedParticipantInfo = sharedData->mParticipantInfo[sharedData->mViewedParticipantIndex];

			// Posición en la pista
			float x = viewedParticipantInfo.mWorldPosition[0] * (-1); //Para invertir
			float y = viewedParticipantInfo.mWorldPosition[2];

			// Velocidad ruedas
			float di_rpm = sharedData->mTyreRPS[0] * (-1);
			float dd_rpm = sharedData->mTyreRPS[1] * (-1);
			float ti_rpm = sharedData->mTyreRPS[2] * (-1);
			float td_rpm = sharedData->mTyreRPS[3] * (-1);

			// Ángulo de la dirección
			float direccion = sharedData->mSteering;
			direccion = direccion * 90;

			// Porcentaje de aceleración
			float throttle = sharedData->mThrottle;
			throttle *= 100;

			// Fuerzas G
			float fuerza_g_x = sharedData->mLocalAcceleration[0]; // Positivo hacia delante, negativo hacia atrás
			float fuerza_g_y = sharedData->mLocalAcceleration[2]; // Positivo hacia la derecha, negativo hacia atrás

			// Tiempo
			float tiempo = sharedData->mCurrentTime;
			tiempo *= 1000;

			float km = sharedData->mOdometerKM;

			// [X, Y, di_rpm, dd_rpm, ti_rpm, td_rpm, direccion, throttle, fuerza_g_x, fuerza_g_y, tiempo]
			myfile << "[" << x << "," << y << ","; // Posicion
			myfile << di_rpm << "," << dd_rpm << "," << ti_rpm << "," << td_rpm << ","; // Velocidad ruedas
			myfile << direccion << "," << throttle << ","; // Direccion y aceleracion
			myfile << fuerza_g_x << "," << fuerza_g_y << ","; // Fuerzas g
			myfile << tiempo << "," << km << "],\n"; // Tiempo en milisegundos

			Sleep(100);

			// Cleanup
			if (_kbhit() != 0) {
				myfile << "]\n";
				myfile.close();
				UnmapViewOfFile(sharedData);
				CloseHandle(fileHandle);

				return 0;
			}
		}
		//printf( "mGameState: (%d)\n", sharedData->mGameState );
		//printf( "mSessionState: (%d)\n", sharedData->mSessionState );
		//printf( "mRaceState: (%d)\n", sharedData->mRaceState );
		//printf("maxRPM: (%f)\n", sharedData->mMaxRPM);
		//printf("mRPM: (%f)\n", sharedData->mRpm);
		//system("cls");
	}
}
