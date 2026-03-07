package main

import (
	"bufio"
	"fmt"
	"os"
	"time"

	"go.bug.st/serial"
)

var mock_mode = true

func log_message(direction string, message string) {
	f, err := os.OpenFile("radio_log.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		fmt.Println("Log error:", err)
		return
	}
	defer f.Close()
	timestamp := time.Now().Format("2006-01-02 15:04:05")
	f.WriteString("[" + timestamp + "] " + direction + ": " + message)
}

func open_serial() serial.Port {
	mode := &serial.Mode{
		BaudRate: 9600,
		Parity:   serial.NoParity,
		DataBits: 8,
		StopBits: serial.OneStopBit,
	}
	port, err := serial.Open("/dev/ttyUSB0", mode)
	if err != nil {
		fmt.Println("Serial error:", err)
		os.Exit(1)
	}
	fmt.Println("Serial port opened")
	return port
}

func send(message string, port serial.Port) {
	if mock_mode {
		fmt.Println("MOCK SENT:", message)
		log_message("SENT", message)
		return
	}
	_, err := port.Write([]byte(message))
	if err != nil {
		fmt.Println("Send error:", err)
		return
	}
	fmt.Println("Sent:", message)
	log_message("SENT", message)
}

func receive(port serial.Port) {
	buf := make([]byte, 256)
	for {
		if mock_mode {
			time.Sleep(1 * time.Second)
			continue
		}
		n, err := port.Read(buf)
		if err != nil {
			fmt.Println("Receive error:", err)
			return
		}
		if n > 0 {
			message := string(buf[:n])
			fmt.Println("\nReceived:", message)
			log_message("RECEIVED", message)
		}
	}
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Println("---Radio Messenger---")

	var port serial.Port
	if !mock_mode {
		port = open_serial()
		defer port.Close()
	}

	go receive(port)

	fmt.Println("Type a message and press Enter to send. Type exit to quit.")
	for {
		fmt.Print("> ")
		input, _ := reader.ReadString('\n')
		if input == "exit\n" {
			fmt.Println("Exiting.")
			break
		}
		send(input, port)
	}
}
