#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


int readPin (int pin) {
    pinMode (pin, OUTPUT);
    digitalWrite (pin, LOW);
    delay(1);
    int c=0;
    pinMode (pin, INPUT);

    while (digitalRead (pin)==LOW) {
        c++;
    }
    return c;
}

int beep (int pid) {
    std::string query = std::to_string (pid);
    std::string hostname = "192.168.31.229";
    struct sockaddr_in sin;
    struct hostent *h;
    int fd;
    ssize_t n_written, remaining;
    char buf[1024];

    h = gethostbyname(hostname.c_str ());
    if (!h) {
        fprintf(stderr, "Couldn't lookup %s: %s", hostname.c_str (), hstrerror(h_errno));
        return 1;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "socket");
        return 1;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(12355);
    sin.sin_addr = *(struct in_addr*)h->h_addr;
    if (connect(fd, (struct sockaddr*) &sin, sizeof(sin))) {
        fprintf(stderr, "connect");
        close(fd);
        return 1;
    }

    send(fd, query.c_str (), query.size (), 0);
    close(fd);
}

void sensorHandler (int pid, int note) {
    int avg_num = 1;
    while (1) {
        int ans = 0;
        for (int avg = 0; avg < avg_num; avg++)
            ans += readPin (pid);
        printf ("pid: %d, delay %d\n", pid, ans / avg_num);
        beep (note);
        delay (100);
    }
}

int main () {
    printf ("Raspberry Pi wiringPi Capacitor reading \n") ;

    if (wiringPiSetup () == -1)
        exit (1) ;

    int sensors_to_listen = 5;

    std::vector<std::thread> sensors;

    int tone = 7;
    for (int i = 0; i < sensors_to_listen; i++) {
        sensors.emplace_back (sensorHandler, i + 1, tone);
        tone = i;
    }
	
    for (auto &s : sensors) {
        s.join ();
    }
}
