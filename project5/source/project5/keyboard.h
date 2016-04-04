#ifndef KB_H_
#define KB_H_

#define KB_PORT PORTA
#define KB_PIN  PINA
#define KB_DDR  DDRA

#define KB_HOLD      1
#define KB_RELEASED  0

void kb_init();
void kb_update();
char kb_getc();
char kb_pos2c(int pos);


#endif /* KB_H_ */