function doQueryCallback(db, query, callback) {
	//console.log(query);
	var queryURL = "http://weatherspot.us/db/query.php?db=" + db + "&query=" + encodeURIComponent(query);
	$.ajax({
		type: 'GET',
		url: queryURL,
		success: function(data) {
			callback(data);
		},
		error: function(jqXHR, textStatus, errorThrown) {
		},
		dataType: "json"
	});
}
function makeSeriesSelectionCallback(data) {
	var appendStr = "<select id='series'>";
	$.each(data[0].points, function(i, e) {
		appendStr += "<option value='" + e[1] + "'>" + e[1] + "</option>";
	});
	appendStr += "<option value='/.*/'>All</option>";
	appendStr += "</select>";
	$('#queryFactory').prepend(appendStr);
}
function makeSeriesSelection() {
	doQueryCallback("weather", "LIST SERIES;", makeSeriesSelectionCallback);
}
function makeMeasurementSelection() {
	var appendStr = "<select id='measurement'>";
	appendStr += "<option value='temperature'>Temperature</option>";
	appendStr += "<option value='humidity'>Humidity</option>";
	appendStr += "<option value='pressure'>Pressure</option>";
	appendStr += "<option value='lighting'>Lighting</option>";
	appendStr += "<option value='*'>All</option>";
	appendStr += "</select>";
	$('#queryFactory').append(appendStr);
}
function makeBeforeOrAfterSelection() {
	var appendStr = "<select class='beforeOrAfter'>";
	appendStr += "<option value='<'>Before</option>";
	appendStr += "<option value='>'>After</option>";
	$('#queryFactory').append(appendStr);
}
var WHERECount = 0;
function makeDateEntryTextbox() {
	var dateStr = "";
	if (WHERECount == 1) {
		$('.beforeOrAfter:last-of-type option:nth-child(2)').prop('selected', true);
		dateStr = todaysDateString(-1);				//Todays date minus 1 day
	}
	else if (WHERECount == 2)
		dateStr = todaysDateString();
	else
		dateStr = "Date...";

	console.log(dateStr);
	var appendStr = "<input value='" + dateStr + "' type='text' class='datePicker' style='width: 10em'></input>";
	$('#queryFactory').append(appendStr);
	$('.datePicker').datepicker( { dateFormat: "yy-mm-dd" } );
}
function makeTimeEntryTextbox() {
	
	var timeStr = "";
	if (WHERECount == 1) {
		timeStr = nowTimeString();
	}
	else if (WHERECount == 2)
		timeStr = nowTimeString();
	else
		timeStr = "Time...";
	var appendStr = "<input value='" + timeStr + "' type='text' class='timePicker' style='width: 7em'></input>"; 
	$('#queryFactory').append(appendStr);
	$('.timePicker').timepicker( { 'timeFormat': 'H:i:s' } );
}
function makeBooleanChoiceSelection() {
	var appendStr = "<select class='booleanChoice'>";
	appendStr += "<option value='and'>And</option>";
	appendStr += "<option value='or'>Or</option>";
	$('#queryFactory').append(appendStr);

}
function nowTimeString() {
	var d = new Date();
	var hours = d.getHours() + getTimeZoneAdjustment("EDT");
	var minutes = d.getMinutes();
	var seconds = d.getSeconds();
	var output = ((''+hours).length<2 ? '0' : '') + hours + ':' +
	    ((''+minutes).length<2 ? '0' : '') + minutes + ':' +
	    ((''+seconds).length<2 ? '0' : '') + seconds;

	return output;
}
function getTimeZoneAdjustment(zone) {
	//Add more timezones!
	switch(zone) {
		case "EDT":	//Eastern Daylight Time UTC-4
			return 4;
		default:
			return 0;
	}
}
function todaysDateString(dayOffset) {
	if (typeof(dayOffset)==='undefined') dayOffset = 0;	//Default parameter to 0
	// Thanks Tadeck@stackoverflow
	// http://stackoverflow.com/questions/8398897/how-to-get-current-date-in-jquery
	var d = new Date();
	var month = d.getMonth()+1;
	var day = d.getDate()+dayOffset;

	var output = d.getFullYear() + '-' +
	    ((''+month).length<2 ? '0' : '') + month + '-' +
	    ((''+day).length<2 ? '0' : '') + day;

	return output;

}
function formatDateTextboxes() {
	if ($('.datePicker').length == 0)
		return "";

	var formatStrArr = [];
	$('.datePicker').each(function(index, elem) {
		formatStrArr[index] = $(elem).val();
		console.log("DATEBOX Index: " + index + " Element: " + $(elem).val());
	});
	return formatStrArr;
}
function formatTimeTextboxes() {
	var formatStrArr = [];
	$('.timePicker').each(function(index, elem) {
		var val = $(elem).val()
		if (val == "Time...")
			val = "00:00:00";
		formatStrArr[index] = val; 
		console.log("TIMEBOX Index: " + index + " Element: " + val);
	});
	return formatStrArr;
}
function formatBeforeOrAfterSelections() {
	var formatStrArr = []
	$('.beforeOrAfter').each(function(index, elem) {
		formatStrArr[index] = $(elem).val();
		console.log("BEFOAFT Index: " + index + " Element: " + $(elem).val());
	});
	return formatStrArr;
}
function formatBooleanSelections() {
	var formatStrArr = []
	$('.booleanChoice').each(function(index, elem) {
		formatStrArr[index] = $(elem).val();
		console.log("BOOLEAN Index: " + index + " Element: " + $(elem).val());
	});
	return formatStrArr;

}
//Build a query string from the components of the queryFactory form
//Returns a proper InfluxDB Query Language query.
function formToQueryString() {
	var queryStr = "SELECT ";
	queryStr += $('#measurement').val() + " FROM ";
	queryStr += $('#series').val();
	var dateStrArr = formatDateTextboxes();
	var timeStrArr = formatTimeTextboxes();
	var beforeOrAfterArr = formatBeforeOrAfterSelections();
	var booleansArr = formatBooleanSelections();
	if (dateStrArr.length > 0) {
		queryStr += " WHERE ";
		for (var index in dateStrArr) {
			if (index > 0) {
				queryStr += " " + booleansArr[index - 1] + " ";
			}
			var timestampStr = "'" + dateStrArr[index] + " " + timeStrArr[index] + "'";

			queryStr += "time " + beforeOrAfterArr[index]; 
			queryStr += timestampStr;
		}
	}
	var limitStr = $('#limitTextbox').val();
	if (limitStr > 0)
		queryStr += " LIMIT " + limitStr;


	console.log("Query Factory String: " + queryStr);

	return queryStr;
}
function makeResetButton() {
	$('#queryFactory').append("<button id='resetBtn' type='button' onclick='resetButtonOnClick()'>Reset</button>");
}
function resetButtonOnClick() {
	$('.beforeOrAfter').remove();
	$('.datePicker').remove();
	$('.timePicker').remove();
	$('.booleanChoice').remove();	
	while (WHERECount > 0) {
		$('#queryFactory br:nth-last-of-type(3)').remove();
		--WHERECount;
	}
	$('#resetBtn').remove();
}

var buttonText = null;
var queryCallback = null;

function WHEREClauseButtonOnClick() {
	$('#queryBtn').remove();
	$('#resetBtn').remove();
	$('#queryFactory br:last-of-type').remove();
	$('#queryFactory br:last-of-type').remove();
	$('#queryFactory p:last-of-type').remove();
	$('#limitTextbox').remove();
	if ($('#queryFactory br').length > 0) {
		makeBooleanChoiceSelection();
	}
	++WHERECount;

	$('#queryFactory').append('<br>');
	makeBeforeOrAfterSelection();
	makeDateEntryTextbox();
	makeTimeEntryTextbox();
	makeWHEREClauseButton();
	$('#queryFactory').append('<br>');
	makeLimitTextbox();
	$('#queryFactory').append('<br>');
	makeQueryButton();
	makeResetButton();
	
}
function makeQueryButton() {
	$('#queryFactory').append("<button id='queryBtn' type='button' onclick='queryCallback(formToQueryString())'>" + buttonText + "</button>");
}

function makeWHEREClauseButton() {
	var appendStr = "<a href='javascript:void(0);' style='display: inline' onclick='WHEREClauseButtonOnClick(); $(this).remove()'>Add Where Clause</a>"
	$('#queryFactory').append(appendStr);
}
function makeLabel() {
	var prependStr = "<p>All Times are UTC</p>";
	$('#factoryContainer').prepend(prependStr);
}
function makeLimitTextbox() {
	var appendStr = "<p style='display:inline; padding-left: 4px'>Query Limit:<input value='1' type='number' id='limitTextbox' style='width: 6em'></input></p>";
	$('#queryFactory').append(appendStr);
	
}
function makeQueryFactory(appendToElem, _buttonText, _queryCallback) {
	buttonText = _buttonText;
	queryCallback = _queryCallback;

	$(appendToElem).append("<div id='factoryContainer' style='outline: pink solid 1px'><div id='queryFactory'></div></div>");
	makeSeriesSelection();
	makeMeasurementSelection();
	makeWHEREClauseButton();
	$('#queryFactory').append('<br>');
	makeLimitTextbox();
	$('#queryFactory').append('<br>');
	makeQueryButton();
	makeLabel();
}
