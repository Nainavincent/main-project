import cv2
import numpy as np
import face_recognition
import os
import serial
import time
arduino = serial.Serial(port='COM5', baudrate=9600, timeout=.1)

from datetime import datetime
import csv

msg = "p6"

face_cascade = cv2.CascadeClassifier('haarcascades/haarcascade_frontalface_default.xml')
path = 'res'
images = []
classNames = []
myList = os.listdir(path)
print(myList)
for cl in myList:
    curImg = cv2.imread(f'{path}/{cl}')
    images.append(curImg)
    classNames.append(os.path.splitext(cl)[0])
print(classNames)


def findEncodings(images):
    encodeList = []
    for img in images:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        encode = face_recognition.face_encodings(img)[0]
        encodeList.append(encode)
    return encodeList



encodeListKnown = findEncodings(images)
print('Encoding complete')

cap = cv2.VideoCapture(0)
while True:
    success, img = cap.read()
    imgS = cv2.resize(img, (0, 0), None, 0.25, 0.25)
    imgS = cv2.cvtColor(imgS, cv2.COLOR_BGR2RGB)

    msg = "p0"
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)  # Convert into grayscale
    faces = face_cascade.detectMultiScale(gray, 1.2, 8)  # Detect faces

    facesCurFrame = face_recognition.face_locations(imgS)
    encodesCurFrame = face_recognition.face_encodings(imgS, facesCurFrame)

    for encodeFace, faceLoc in zip(encodesCurFrame, facesCurFrame):
        matches = face_recognition.compare_faces(encodeListKnown, encodeFace)
        faceDis = face_recognition.face_distance(encodeListKnown, encodeFace)
        #print(faceDis)
        matchIndex = np.argmin(faceDis)

        if matches[matchIndex]:
            name = classNames[matchIndex].upper()
            #print(name)
            y1, x2, y2, x1 = faceLoc
            y1, x2, y2, x1 = y1 * 4, x2 * 4, y2 * 4, x1 * 4
            cv2.rectangle(img, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.rectangle(img, (x1, y2 - 35), (x2, y2), (0, 255, 0), cv2.FILLED)
            cv2.putText(img, name, (x1 + 6, y2 - 6), cv2.FONT_HERSHEY_COMPLEX, .5, (255, 255, 255), 1)
            msg = "p1"
        else:
            for (x, y, w, h) in faces:  # Draw rectangle around the faces
                cv2.rectangle(img, (x, y), (x + w, y + h), (0, 0, 255), 3)
                cv2.putText(img, "Unknown", (x + 6, y - 6), cv2.FONT_HERSHEY_COMPLEX, .5, (0, 0, 255), 1)
                msg = "p2"

    data = arduino.readline()
    if data:
        data = data.decode()
        print(data[0])
        #print(data[1])
        print(type(data))
        if data[0] == "a":
            arduino.write(bytes(msg, 'utf-8'))
            time.sleep(0.05)
            print(msg)
    cv2.imshow('Webcam', img)
    cv2.waitKey(1)
