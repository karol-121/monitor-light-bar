function timeToTimestamp(inputTime) {

    const hours = inputTime.substring(0,2);
    const minutes = inputTime.substring(3,5);
    return ((hours * 60) + minutes) * 60000;
    
}

function encodeNumberToFixedLenght(int) {

    if (int < 0 || int > 999) {
        return "000";
    } else if (int <10) {
        return "00"+int;
    } else if (int < 100) {
        return "0"+int;
    } else {
        return int;
    }

}

function stringToBoolean(string) {

    if (string === "true") {
        return true;
    } else if (string ==="false") {
        return false;
    } else {
        return null;
    }

}