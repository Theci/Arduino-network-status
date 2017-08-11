package main

import (
	"bytes"
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
)

var (
	alertm Alertmanager
	status map[string]string
)

// ArduinoPayload payload sized for arduino
type ArduinoPayload struct {
	Target string `json:"target"`
	Status string `json:"status"`
}

// Alertmanager holds data coming from the Alertmanager webhook
type Alertmanager struct {
	Status string  `json:"status"`
	Alerts []Alert `json:"alerts"`
}

// Alert holds a single alert
type Alert struct {
	Labels interface{} `json:"labels"`
}

func main() {
	status = make(map[string]string)
	http.HandleFunc("/alerts", HandleIncoming)
	log.Fatal(http.ListenAndServe(":18081", nil))
}

func prog() {
	AlertmanagerUpdate(&status)
	SendArduinoMap(&status)
}

// HandleIncoming handles incoming requests from Alertmanager
func HandleIncoming(w http.ResponseWriter, req *http.Request) {
	json.NewDecoder(req.Body).Decode(&alertm)
	req.Body.Close()
	prog()
	return
}

// AlertmanagerUpdate update the status map with new data from Alertmanager
func AlertmanagerUpdate(m *map[string]string) {
	targetStat := "10.3.21.14"
	log.Print("Updating map ...")
	for i := range alertm.Alerts {
		alert := alertm.Alerts[i]
		//target := (alert.Labels.(map[string]interface{})["instance"]).(string)
		if alertm.Status == "resolved" {
			(*m)[targetStat] = "ok"
		} else {
			(*m)[targetStat] = (alert.Labels.(map[string]interface{})["severity"]).(string)
		}
	}
	log.Print("Map updated:")
	log.Println(*m)
}

// SendArduinoMap process the targets in the map and send the data to the arduinos
func SendArduinoMap(m *map[string]string) {
	for k, v := range *m {
		json := ArduinoPayload{Target: k, Status: v}
		SendArduino(&k, &json)
	}
}

// SendArduino : POST the Arduino payload to the targeted Arduino
func SendArduino(target *string, data *ArduinoPayload) {
	buf := new(bytes.Buffer)
	json.NewEncoder(buf).Encode(*data)
	resp, err := http.Post("http://"+*target+":18080", "application/json", buf)
	if err != nil {
		panic(err)
	}
	defer resp.Body.Close()

	log.Println("response Status:", resp.Status)
	log.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	log.Println("response Body:", string(body))
}
