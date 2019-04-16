#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <conio.h>
#include <i86.h>
#include <sys/irqinfo.h>
#include <sys/kernel.h>
#include <sys/proxy.h>
#include <sys/sched.h>


typedef uint8_t bool;

#define true  1
#define false 0

#define  PORT_DATA      0x60
#define  PORT_STAT_CMD  0x64

#define  STATUS_OUT_BUF    0x01
#define  STATUS_IN_BUF     0x02
#define  STATUS_SYSFLAG    0x04
#define  STATUS_CMDDATA    0x08
#define  STATUS_TIMEOUT    0x40
#define  STATUS_PARITY_ERR 0x80

#define  inb(x)  inp(x)
#define  outb(x, y) outp(y, x)

void write_cmd(uint8_t byte1, uint8_t byte2, bool two) {
  outb(byte1, PORT_STAT_CMD);
  if (two) {
    outb(byte2, PORT_DATA);
  }
}

uint8_t read_data(void) {
  return inb(PORT_DATA);
}

uint8_t read_status() {
  return inb(PORT_STAT_CMD);
}

void print_status(void) {
  uint8_t data = read_status();

  printf("========================================================\n");
  printf(" Status\n");
  printf("========================================================\n\n");

  printf("Output buffer:\t");
  if (data & STATUS_OUT_BUF) {
    printf("Full\n");
  } else {
    printf("Empty\n");
  }

  printf("Input buffer:\t");
  if (data & STATUS_IN_BUF) {
    printf("Full\n");
  } else {
    printf("Empty\n");
  }

  printf("System flag:\t");  
  if (data & STATUS_IN_BUF) {
    printf("Set\n");
  } else {
    printf("Clear\n");
  }

  printf("Timeout:\t");
  if (data & STATUS_TIMEOUT) {
    printf("Error\n");
  } else {
    printf("Clear\n");
  }

  printf("Parity:\t\t");
  if (data & STATUS_PARITY_ERR) {
    printf("Error\n");
  } else {
    printf("Clear\n");
  }

  printf("\n");
}

uint8_t read_config(void) {
  write_cmd(0x20, 0x00, false);
  return read_data();
}

void write_config(uint8_t config) {
  write_cmd(0x60, config, true);
}

void print_config(uint8_t config) {

  printf("========================================================\n");
  printf(" Config\n");
  printf("========================================================\n\n");
  
  printf("First PS/2 port interrupt:\t");
  if (config & 0x01) {
    printf("enabled\n");
  } else {
    printf("disabled\n");
  }

  printf("Second PS/2 port interrupt:\t");
  if (config & 0x02) {
    printf("enabled\n");
  } else {
    printf("disabled\n");
  }

  printf("System Flag:\t\t\t");
  if (config & 0x04) {
    printf("system passed\n");
  } else {
    printf("0\n");
  }

  printf("First PS/2 port clock:\t\t");
  if (config & 0x20) {
    printf("disabled\n");
  } else {
    printf("enabled\n");
  }

  printf("Second PS/2 port clock:\t\t");
  if (config & 0x40) {
    printf("disabled\n");
  } else {
    printf("enabled\n");
  }

  printf("First PS/2 port translation:\t");
  if (config & 0x80) {
    printf("enabled\n");
  } else {
    printf("disabled\n");
  }
}

void test_controller(void) {
  write_cmd(0xAA, 0x00, false);
  printf("Self-test:\t%02X\n", read_data());
}

void keyb_send(uint8_t data) {
  uint8_t status;

  do {
    status = read_status();
  } while(status & STATUS_IN_BUF);

  outb(data, 0x60);
}

uint8_t keyb_read(void) {
  uint8_t status;

  do {
    status = read_status();
  } while(!(status & STATUS_OUT_BUF));

  return inb(0x60);
}

void keyb_cycle() {
  printf("Keyboard reset\n");
  keyb_send(0xFF);
  printf("%02X\n", inb(0x60));
  printf("%02X\n", inb(0x60));

  printf("Set scan code set 3\n");
  keyb_send(0xF0);
  printf("%02X\n", inb(0x60));
  keyb_send(0x03);
  printf("%02X\n", inb(0x60));

  printf("Set repeat rate\n");
  keyb_send(0xF3);
  printf("%02X\n", inb(0x60));
  keyb_send(0x00);
  printf("%02X\n", inb(0x60));

  printf("Enable scan:\n");

  keyb_send(0xF4);
  printf("%02X\n", inb(0x60));

  printf("Cycle:\n");

  while(1) {
    uint8_t data = keyb_read();
    printf("Scan code: %02X\n", data);
    delay(10);
  }

}

void mouse_send(uint8_t data) {
  outb(0xD4, 0x64);

  uint8_t status;

  do {
    status = read_status();
  } while(status & STATUS_IN_BUF);

  outb(data, 0x60);
}

uint8_t mouse_read(void) {
  //outb(0xD4, 0x64);

  uint8_t status;

  do {
    status = read_status();
  } while(!(status & STATUS_OUT_BUF));
  
  return inb(0x60);
}

void mouse_reset(void) {
  printf("Reset mouse:\n");
  mouse_send(0xFF);
  printf("%02X\n", mouse_read());
  printf("%02X\n", mouse_read());
  printf("%02X\n", mouse_read());
}

void mouse_rate(uint8_t rate) {
  printf("Set samplerate %d:\n", rate);
  mouse_send(0xF3);
  printf("%02X\n", mouse_read());
  mouse_send(rate);
  printf("%02X\n", mouse_read());
}

void mouse_read_type(void) {
  printf("Read type:\n");
  mouse_send(0xF2);
  printf("%02X\n", mouse_read());
  printf("%02X\n", mouse_read());
}

void mouse_res(uint8_t res) {
  printf("Set resolution %d:\n", res);
  mouse_send(0xE8);
  printf("%02X\n", mouse_read());
  mouse_send(res);
  printf("%02X\n", mouse_read());
}

void mouse_scale_11(void) {
  printf("Set scale 1:1:\n");
  mouse_send(0xE6);
  printf("%02X\n", mouse_read());
}

void mouse_stream_mode(void) {
  printf("Set stream mode:\n");
  mouse_send(0xEA);
  printf("%02X\n", mouse_read());  
}

void mouse_en(void) {
  printf("Set enable:\n");
  mouse_send(0xF4);
  printf("%02X\n", mouse_read());
}

pid_t             proxy;
volatile uint8_t  irq_data;

pid_t far isr_handler() {
  //irq_data = inb(0x60);

  return proxy;
}

void mouse_cycle(void) {
  mouse_reset();
  mouse_reset();
  mouse_reset();
  mouse_read_type();

  mouse_rate(200);
  mouse_rate(100);
  mouse_rate(80);
  mouse_read_type();

  mouse_res(3);
  mouse_scale_11();
  mouse_rate(40);
  mouse_stream_mode();
  mouse_en();

  if ((proxy = qnx_proxy_attach(0, 0, 0, -1)) == -1) {
    printf( " Unable to attach proxy. " );
    return;
  }

  int id;

  if ((id = qnx_hint_attach(12, &isr_handler, FP_SEG(&irq_data))) == -1) {
    printf( "Unable to attach interrupt." );
    return;
  }


  while (1) {
    Receive(proxy, 0, 0);
    uint8_t status = inb(0x60);;

    Receive(proxy, 0, 0);
    uint8_t x_move = inb(0x60);;

    Receive(proxy, 0, 0);
    uint8_t y_move = inb(0x60);;

    Receive(proxy, 0, 0);
    uint8_t z_move = inb(0x60);;

    printf("Status: %02X, X: %02X, Y: %02X, Z: %02X\n", status, x_move, y_move, z_move);

    //Receive(proxy, 0, 0 );
    //printf("MOUSE\n");
    /*
    return;
    delay(20);*/
  }

}

int main() {
  //test_controller();

/*  uint8_t status = read_status();

  while (status & STATUS_OUT_BUF) {
    inb(0x60);
    status = read_status();
  }*/

 // write_cmd(0xAD, 0x00, false);
 // write_cmd(0xA7, 0x00, false);
 // write_cmd(0x60, 0x02, true);

  print_status();
  uint8_t config = read_config();
  config |= 0x02; // enable port 2 irq
  write_config(config);

  config = read_config();
  print_config(config);

  //keyb_cycle();
  mouse_cycle();

  return 0;
}
  