#include"SONAR.h"
//static vars
const unsigned char SONAR::addrCmdTemplate[]={0x55,0xaa,0xab,0x01,0x55,0xff,0x00};
// const unsigned char SONAR::addrDatTemplate[]={0x55,0xaa,0xff,0x01,0x55,0xff,0xff};
const unsigned char SONAR::trigCmdTemplate[]={0x55,0xaa,0xff,0x00,0x01,0x00};
const unsigned char SONAR::distCmdTemplate[]={0x55,0xaa,0xff,0x00,0x02,0x00};
//const unsigned char SONAR::distDattemplate[]={0x55,0xaa,0xff,0x02,0x02,0xff,0xff,0xff};
const unsigned char SONAR::tempCmdTemplate[]={0x55,0xaa,0xff,0x00,0x03,0x00};
//const unsigned char SONAR::tempDatTemplate[]={0x55,0xaa,0xff,0x02,0x03,0xff,0xff,0xff};

unsigned char SONAR::_pinCtrl;
//bool SONAR::_mode;
//unsigned char SONAR::_recvBuf[bufSize];


SONAR::SONAR(unsigned char addr) {
	debug();
	initAddr(addr);
}

unsigned char SONAR::initAddr(unsigned char addr) {
	debug();
	if(0x11<=addr && addr<=0x30) _addr=addr;
	generateTrigCmd();
	generateDistCmd();
	return getAddr();
}

unsigned char SONAR::getAddr() const {
	debug();
	return _addr;
}


unsigned char SONAR::setAddr(unsigned char addr) {
	debug();
	if(addr==0) {
		unsigned startTime=millis();
		unsigned int delta=0;
		bool gotKey=false;

		pinMode(keyS7, INPUT);	// 201209
		while(digitalRead(keyS7)==KEYPRESSED) {
			gotKey=true;
			delay(100);			
			SonarPrint.println(delta=millis()-startTime,DEC);
		}
		if(gotKey) {
			setTX();
			SonarPrint.print("GOt KeyS7(ms): ");
			SonarPrint.println(delta,DEC);
		}
		
		delta/=1000;
		if(0x1<=delta && delta<=0x30) addr=0x10+delta;
	}

	unsigned char addrCmd[sizeof(addrCmdTemplate)];
	if(0x11<=addr && addr<=0x30) {
		generateAddrCmd(addrCmd,addr);
		sendCmd(addrCmd,sizeof(addrCmd));
		delay(1);
		recvDat(addrDatSize);
		#ifdef DEBUG
			showDat(addrDatSize);
		#endif
		if(checksum(addrDatSize)==0) {
			initAddr(addr);
			return addr;
		}
	}
	return 0;
}

unsigned char SONAR::getPinCtrl() {
	return _pinCtrl;
}
unsigned char SONAR::setPinCtrl(unsigned char pinCtrl) {
	_pinCtrl=pinCtrl;
	return getPinCtrl();
}


//bool SONAR::getMode() {
//	debug();
//	return _mode;
//}
bool SONAR::setMode(bool mode) {
	debug();
	//_mode=mode;
	digitalWrite(getPinCtrl(),mode);
	//return getMode();
	return mode;
}
bool SONAR::setTX() {
	debug();
	return setMode(MODETX);
}
bool SONAR::setRX() {
	debug();
	return setMode(MODERX);
}

unsigned char* SONAR::generateAddrCmd(unsigned char* addrCmd,unsigned char addr) {
	debug();
	for(int i=0;i<sizeof(addrCmdTemplate);++i) {
		addrCmd[i]=addrCmdTemplate[i];
	}
	addrCmd[sizeof(addrCmdTemplate)-2]=addr;	// addr
	for(int i=0;i<sizeof(addrCmdTemplate)-1;++i) {	// checksum
		addrCmd[sizeof(addrCmdTemplate)-1]+=addrCmd[i];
	}
	return addrCmd;
}
unsigned char* SONAR::generateTrigCmd() {
	debug();
	for(int i=0;i<sizeof(trigCmdTemplate);++i) {
		_trigCmd[i]=trigCmdTemplate[i];
	}
	_trigCmd[2]=getAddr();
	for(int i=0;i<sizeof(trigCmdTemplate)-1;++i) {
		_trigCmd[sizeof(trigCmdTemplate)-1]+=_trigCmd[i];
	}
	return _trigCmd;
}
unsigned char* SONAR::generateDistCmd() {
	debug();
	for(int i=0;i<sizeof(distCmdTemplate);++i) {
		_distCmd[i]=distCmdTemplate[i];
	}
	_distCmd[2]=getAddr();
	for(int i=0;i<sizeof(distCmdTemplate)-1;++i) {
		_distCmd[sizeof(distCmdTemplate)-1]+=_distCmd[i];
	}
	return _distCmd;
}
unsigned char* SONAR::generateTempCmd(unsigned char* tempCmd,unsigned char addr) {
	debug();
	for(int i=0;i<sizeof(tempCmdTemplate);++i) {
		tempCmd[i]=tempCmdTemplate[i];
	}
	tempCmd[2]=addr;
	for(int i=0;i<sizeof(tempCmdTemplate)-1;++i) {
		tempCmd[sizeof(tempCmdTemplate)-1]+=tempCmd[i];
	}
	return tempCmd;
}

unsigned char SONAR::sendCmd(unsigned char* cmd,unsigned char size) {
	debug();
	init();	// 201209
	setTX();
	for(int i=0;i<size;++i) {
		//Serial.print(cmd[i]);
		Serial.write(cmd[i]);	// 201204
	}
	Serial.flush(); // 201204
	return size;
}
unsigned char SONAR::clearBuf() {
	debug();
	for(int i=0;i<sizeof(_recvBuf);++i) {
		_recvBuf[i]=0;
	}
	return sizeof(_recvBuf);
}
unsigned char SONAR::recvDat(unsigned char desiredSize) {
	debug();
	clearBuf();
	init();	// 201209
	setRX();
	unsigned char datSize=0;
	
#if defined(BOARD_maple) || defined(BOARD_maple_native) || defined(BOARD_maple_mini)
	for(int j=0;datSize<desiredSize && j<15;++j) {
		delay(1);
		unsigned char ibyte=Serial.read();
		
		if(datSize==0 && ibyte!=0x55) continue;
		if(datSize==1 && ibyte!=0xaa) continue;
		_recvBuf[datSize++]=ibyte;
	}
#else//for arduino
	for(int j=0;datSize<desiredSize && j<5000;++j) {
		unsigned char ibyte=Serial.read();
		if(ibyte!=0xff) {
			_recvBuf[datSize++]=ibyte;
		}
	}
#endif

	//SonarPrint.println(Serial.read(), HEX);
	setTX();
	return datSize;
}
char SONAR::checksum(unsigned char desiredSize) {
	debug();
	if(_recvBuf[0]==0) return -1;
	unsigned char sum=0;
	for(int i=0;i<desiredSize-1;++i) {
		sum+=_recvBuf[i];
	}
	if(sum!=_recvBuf[desiredSize-1]) return -1;
	//if(_recvBuf[2]!=getAddr()) return -1; //
	return 0;
}
unsigned char SONAR::showDat(unsigned char desiredSize) {
	debug();
	setTX();
	for(int i=0;i<desiredSize;++i) {
		SonarPrint.print(_recvBuf[i],HEX);
		SonarPrint.print(" ");
	}
	SonarPrint.println();
	return desiredSize; 
}
void SONAR::init(unsigned char pinCtrl,unsigned int baudrate) {
	debug();
	Serial.begin(baudrate);
	setPinCtrl(pinCtrl);
	pinMode(getPinCtrl(),OUTPUT);
	//pinMode(keyS7, INPUT);
	setTX();
}
void SONAR::release() {
	debug();
	Serial.end();
}
unsigned char SONAR::trigger() {
	debug();
	return sendCmd(_trigCmd,sizeof(_trigCmd));
}
unsigned int SONAR::getDist() {
	debug();
	
#if defined(BOARD_maple) || defined(BOARD_maple_native) || defined(BOARD_maple_mini)
	Serial.flush();
#endif

	sendCmd(_distCmd,sizeof(_distCmd));
	delay(1);
	recvDat(distDatSize);
	#ifdef DEBUG
		showDat(distDatSize);
	#endif
	
	if(checksum(distDatSize)==0) return (_recvBuf[5]<<8)+_recvBuf[6];

	return 0xffff;	// not available distance
}
int SONAR::getTemp() {
	debug();
	unsigned char tempCmd[sizeof(tempCmdTemplate)];
	generateTempCmd(tempCmd,getAddr());

#if defined(BOARD_maple) || defined(BOARD_maple_native) || defined(BOARD_maple_mini)
	Serial.flush();
#endif

	sendCmd(tempCmd,sizeof(tempCmd));
	delay(1);
	recvDat(tempDatSize);
	#ifdef DEBUG
		showDat(tempDatSize);
	#endif
	if(checksum(tempDatSize)==0) {
		int temp=(((_recvBuf[5]&0x0f)<<8)+_recvBuf[6])/10;
		if((_recvBuf[5]&0xf0)==0) return temp;
		return -1*temp;
	}
	return 0xffff;
}
/*
float SONAR::getTemp() {	// to reduce binary code, use int, instead of float
	debug();
	unsigned char tempCmd[sizeof(tempCmdTemplate)];
	generateTempCmd(tempCmd,getAddr());
	sendCmd(tempCmd,sizeof(tempCmd));
	delay(1);
	recvDat(tempDatSize);
	showDat(tempDatSize);
	if(checksum(tempDatSize)==0) {
		float temp=0.1*(((_recvBuf[5]&0x0f)<<8)+_recvBuf[6]);
		if((_recvBuf[5]&0xf0)==0) return temp;
		return -1*temp;
	}
	return 0xffff;
}
*/
