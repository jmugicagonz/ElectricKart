void setup() {
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  pinMode(A6,INPUT);
  pinMode(A7,INPUT);
  pinMode(A8,INPUT);
  pinMode(A9,INPUT);
  pinMode(A10,INPUT);
  pinMode(A11,INPUT);
  pinMode(A12,OUTPUT);
  pinMode(A13,OUTPUT);
  pinMode(A14,OUTPUT); 
  

}

void loop() {
  int cutCurrent=0;
  int cutCurrCh=0;
  int cutCurrDisch=0;
  //if((digitalRead(A0)==LOW)||(digitalRead(A3)==LOW)||(digitalRead(A6)==LOW)||(digitalRead(A9)==LOW)){
  if((digitalRead(A0)==LOW)||(digitalRead(A3)==LOW)){
    cutCurrent++;
  }
  //if((digitalRead(A1)==LOW)||(digitalRead(A4)==LOW)||(digitalRead(A7)==LOW)||(digitalRead(A10)==LOW)){
  if((digitalRead(A1)==LOW)||(digitalRead(A4)==LOW)){
    cutCurrCh++;
  }
  //if((digitalRead(A2)==LOW)||(digitalRead(A5)==LOW)||(digitalRead(A8)==LOW)||(digitalRead(A11)==LOW)){
  if((digitalRead(A2)==LOW)||(digitalRead(A5)==LOW)){
    cutCurrDisch++;
  }
  if(cutCurrent!=0){
    cutRelayCh();
    cutRelayDisch();
  }
  else if((cutCurrent==0)&&(cutCurrCh!=0)&&(cutCurrDisch==0)){
    cutRelayCh();
    restoreRelayDisch();
  }
  else if((cutCurrent==0)&&(cutCurrCh!=0)&&(cutCurrDisch!=0)){
    cutRelayCh();
    cutRelayDisch();
  }
  else if((cutCurrent==0)&&(cutCurrCh==0)&&(cutCurrDisch!=0)){
    restoreRelayCh();
    cutRelayDisch();
  }
  else if((cutCurrent==0)&&(cutCurrCh==0)&&(cutCurrDisch==0)){
    restoreRelayCh();
    restoreRelayDisch();
  }
  

}

void cutRelayCh(){
  digitalWrite(A12,LOW);
  digitalWrite(A14,HIGH); 
  return;
}
void restoreRelayCh(){
  digitalWrite(A12,HIGH);
  digitalWrite(A14,LOW);
  return;
}
void cutRelayDisch(){
  digitalWrite(A13,LOW);
  return;
}
void restoreRelayDisch(){
  digitalWrite(A13,HIGH);
  return;
}
