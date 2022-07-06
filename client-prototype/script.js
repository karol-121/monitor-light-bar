const deviceIp = "http://192.168.0.115";

//define dom elements
let leftBrightnessRange;
let leftBrightnessSpan;
let rightBrightnessRange;
let rightBrightnessSpan;
let powerOnHourCheckbox;
let powerOnHourRange;
let powerOnHourSpan;

//global variables that should be saved 
let leftBrightness;
let rightBrightness;
let power;
let powerOnHourEnabled;
let powerOnHour;

//these are global for session.
let intervalID;
let timeoutID;

$(function () {
    //assign dom elements
    leftBrightnessRange = $("#left_brightness");
    leftBrightnessSpan = $("#left_brightness_value");
    rightBrightnessRange = $("#right_brightness");
    rightBrightnessSpan = $("#right_brightness_value");
    powerOnHourCheckbox = $("#power_on_hour_flag");
    powerOnHourRange = $("#power_on_hour");
    powerOnHourSpan = $("#power_on_hour_value");
    
    //here do a call for get status. this will not be needed tho. 

    //check if values are saved in localstorage, this is not ideal as it does not check all values.
   if (localStorage.getItem("leftBrightness")) {

       

        leftBrightness = localStorage.getItem("leftBrightness");
        rightBrightness = localStorage.getItem("rightBrightness");
        power = localStorage.getItem("power");
        powerOnHourEnabled = stringToBoolean(localStorage.getItem("powerOnHourEnabled"));
        powerOnHour = localStorage.getItem("powerOnHour");

        console.log(powerOnHourEnabled);

   } else { //if there is no localstorage items, then set default values
        leftBrightness = 255;
        rightBrightness = 255;
        power = 0;
        powerOnHourEnabled = false;
        powerOnHour = 12;
   }

    //set these values as default for gui elements
    leftBrightnessRange.val(leftBrightness);
    leftBrightnessSpan.text(leftBrightness);

    rightBrightnessRange.val(rightBrightness);
    rightBrightnessSpan.text(rightBrightness);

    powerOnHourCheckbox.prop("checked", powerOnHourEnabled); 
    powerOnHourRange.val(powerOnHour);
    powerOnHourSpan.text(powerOnHour);

    //starting "service" functions
    updatePowerOnHourTimeout();

});

function setLeftBrightness() {
    leftBrightness = leftBrightnessRange.val();
    leftBrightnessSpan.text(leftBrightness);
    setBrightness();
}

function setRightBrightness() {
    rightBrightness = rightBrightnessRange.val();
    rightBrightnessSpan.text(rightBrightness);
    setBrightness();
}

function setBrightness() {
    //this function will call the device to set brightnesses
    console.log("http request: set brightness | values: "+ encodeNumberToFixedLenght(leftBrightness) + " " + encodeNumberToFixedLenght(rightBrightness));

    $.ajax({
        url: deviceIp+"/brightness?left="+encodeNumberToFixedLenght(leftBrightness)+"&right="+encodeNumberToFixedLenght(rightBrightness),
        type: "GET",
        crossDomain: true
    });
}

function powerOn() {
    power = 1;
    setPower();

}

function powerOff() {
    power = 0;
    setPower();
}

function setPower() {
    //this function will call the device to set the power status
    console.log("http request: set power | value: "+power);

    $.ajax({
        url: deviceIp+"/?power="+power,
        type: "GET",
        crossDomain: true
    });
}

//function that sets the 
function enablePowerOnHour() {

    //check if power on hour should be on
    if (powerOnHourCheckbox.is(":checked")) {
        
        powerOnHourEnabled = true;
        powerOnHourRange.prop("disabled", false);
        updatePowerOnHourTimeout()

    } else {

        powerOnHourEnabled = false;
        powerOnHourRange.prop("disabled", true);
        updatePowerOnHourTimeout();

    }

}

//function that sets the global value of power on hour
function setPowerOnHour() {

    powerOnHour = powerOnHourRange.val();
    powerOnHourSpan.text(powerOnHour);
    updatePowerOnHourTimeout();

}

//function that sets the timeout if possible
function updatePowerOnHourTimeout() {

    clearTimeout(timeoutID); //clear timeout if exist from before

    //if timeout should be set
    if (powerOnHourEnabled) {
        const powerOnEvent = new Date();
        timeoutID = setTimeout(powerOn, (powerOnEvent.setHours(powerOnHour, 0, 0, 0) - Date.now()));
    }

}

//function that takes current global values and save them to local storage.
function save() {

    console.log(powerOnHourEnabled);
   
   localStorage.setItem("leftBrightness", leftBrightness);
   localStorage.setItem("rightBrightness", rightBrightness);
   localStorage.setItem("power", power);
   localStorage.setItem("powerOnHourEnabled", powerOnHourEnabled);
   localStorage.setItem("powerOnHour", powerOnHour);


}
