function doQuery(db, query, callback) {
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
	doQuery("weather", "LIST SERIES;", makeSeriesSelectionCallback);
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
function makeDateEntryTextbox() {
	var appendStr = "<input value='Date...' type='text' class='datePicker' style='width: 10em'></input>";
	$('#queryFactory').append(appendStr);
	$('.datePicker').datepicker( { dateFormat: "yy-mm-dd" } );
}
function makeTimeEntryTextbox() {
	var appendStr = "<input value='Time...' type='text' class='timePicker' style='width: 7em'></input>"; 
	$('#queryFactory').append(appendStr);
	$('.timePicker').timepicker( { 'timeFormat': 'H:i:s' } );
}
function makeBooleanChoiceSelection() {
	var appendStr = "<select class='booleanChoice'>";
	appendStr += "<option value='and'>And</option>";
	appendStr += "<option value='or'>Or</option>";
	$('#queryFactory').append(appendStr);

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


	console.log("Query Factory String: " + queryStr);

	graphQuery(queryStr);
}

function WHEREClauseButtonOnClick() {
	$('#graphQueryBtn').remove()
	$('#queryFactory br:last-child').remove();
	if ($('#queryFactory br').length > 0) {
		makeBooleanChoiceSelection();
	}

	$('#queryFactory').append('<br>');
	makeBeforeOrAfterSelection();
	makeDateEntryTextbox();
	makeTimeEntryTextbox();
	makeWHEREClauseButton();
	$('#queryFactory').append('<br>');
	makeGraphQueryButton();
	
}
function makeGraphQueryButton() {
	$('#queryFactory').append("<button id='graphQueryBtn' type='button' onclick='formToQueryString()'>Graph Query</button>");
}

function makeWHEREClauseButton() {
	var appendStr = "<a href='javascript:void(0);' style='display: inline' onclick='WHEREClauseButtonOnClick(); $(this).remove()'>Add Where Clause</a>"
	$('#queryFactory').append(appendStr);
}
function makeQueryFactory() {
	$('body').append("<div id='queryFactory'></div>");
	makeSeriesSelection();
	makeMeasurementSelection();
	makeWHEREClauseButton();
	$('#queryFactory').append('<br>');
	makeGraphQueryButton();
}
