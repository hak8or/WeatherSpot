var weatherspot_queryFactory = {
	WHERECount: 0,
	buttonText: null,
	queryCallback: null,
	doQueryCallback: function(db, query, callback) {
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
	},
	makeSeriesSelectionCallback: function(data) {
		var appendStr = "<select id='series'>";
		$.each(data[0].points, function(i, e) {
			appendStr += "<option value='" + e[1] + "'>" + e[1] + "</option>";
		});
		appendStr += "<option value='/.*/'>All</option>";
		appendStr += "</select>";
		$('#queryFactory').prepend(appendStr);
	},
	makeSeriesSelection: function() {
		this.doQueryCallback("weather", "LIST SERIES;", this.makeSeriesSelectionCallback);
	},
	makeMeasurementSelection: function() {
		var appendStr = "<select id='measurement'>";
		appendStr += "<option value='temperature'>Temperature</option>";
		appendStr += "<option value='humidity'>Humidity</option>";
		appendStr += "<option value='pressure'>Pressure</option>";
		appendStr += "<option value='lighting'>Lighting</option>";
		appendStr += "<option value='*'>All</option>";
		appendStr += "</select>";
		$('#queryFactory').append(appendStr);
	},
	makeBeforeOrAfterSelection: function() {
		var appendStr = "<select class='beforeOrAfter'>";
		appendStr += "<option value='<'>Before</option>";
		appendStr += "<option value='>'>After</option>";
		$('#queryFactory').append(appendStr);
	},
	makeDateEntryTextbox: function() {
		var dateStr = "";
		if (this.WHERECount == 1) {
			$('.beforeOrAfter:last-of-type option:nth-child(2)').prop('selected', true);
			dateStr = this.todaysDateString(-1);				//Todays date minus 1 day
		}
		else if (this.WHERECount == 2)
			dateStr = this.todaysDateString();
		else
			dateStr = "Date...";

		//console.log(dateStr);
		var appendStr = "<input value='" + dateStr + "' type='text' class='datePicker' style='width: 10em'></input>";
		$('#queryFactory').append(appendStr);
		$('.datePicker').datepicker( { dateFormat: "yy-mm-dd" } );
	},
	makeTimeEntryTextbox: function() {
		
		var timeStr = "";
		if (this.WHERECount == 1) {
			timeStr = this.nowTimeString();
		}
		else if (this.WHERECount == 2)
			timeStr = this.nowTimeString();
		else
			timeStr = "Time...";
		var appendStr = "<input value='" + timeStr + "' type='text' class='timePicker' style='width: 7em'></input>"; 
		$('#queryFactory').append(appendStr);
		$('.timePicker').timepicker( { 'timeFormat': 'H:i:s' } );
	},
	makeBooleanChoiceSelection: function() {
		var appendStr = "<select class='booleanChoice'>";
		appendStr += "<option value='and'>And</option>";
		appendStr += "<option value='or'>Or</option>";
		$('#queryFactory').append(appendStr);
	},
	nowTimeString: function() {
		var d = new Date();
		var hours = d.getHours() + this.getTimeZoneAdjustment("EDT");
		var minutes = d.getMinutes();
		var seconds = d.getSeconds();
		var output = ((''+hours).length<2 ? '0' : '') + hours + ':' +
		    ((''+minutes).length<2 ? '0' : '') + minutes + ':' +
		    ((''+seconds).length<2 ? '0' : '') + seconds;

		return output;
	},
	getTimeZoneAdjustment: function(zone) {
		//Add more timezones!
		switch(zone) {
			case "EDT":	//Eastern Daylight Time UTC-4
				return 4;
			default:
				return 0;
		}
	},
	todaysDateString: function(dayOffset) {
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

	},
	formatDateTextboxes: function() {
		if ($('.datePicker').length == 0)
			return "";

		var formatStrArr = [];
		$('.datePicker').each(function(index, elem) {
			formatStrArr[index] = $(elem).val();
			//console.log("DATEBOX Index: " + index + " Element: " + $(elem).val());
		});
		return formatStrArr;
	},
	formatTimeTextboxes: function() {
		var formatStrArr = [];
		$('.timePicker').each(function(index, elem) {
			var val = $(elem).val()
			if (val == "Time...")
				val = "00:00:00";
			formatStrArr[index] = val; 
			//console.log("TIMEBOX Index: " + index + " Element: " + val);
		});
		return formatStrArr;
	},
	formatBeforeOrAfterSelections: function() {
		var formatStrArr = []
		$('.beforeOrAfter').each(function(index, elem) {
			formatStrArr[index] = $(elem).val();
			//console.log("BEFOAFT Index: " + index + " Element: " + $(elem).val());
		});
		return formatStrArr;
	},
	formatBooleanSelections: function() {
		var formatStrArr = []
		$('.booleanChoice').each(function(index, elem) {
			formatStrArr[index] = $(elem).val();
			//console.log("BOOLEAN Index: " + index + " Element: " + $(elem).val());
		});
		return formatStrArr;

	},
	//Build a query string from the components of the queryFactory form
	//Returns a proper InfluxDB Query Language query.
	formToQueryString: function() {
		var queryStr = "SELECT ";
		queryStr += $('#measurement').val() + " FROM ";
		queryStr += $('#series').val();
		var dateStrArr = this.formatDateTextboxes();
		var timeStrArr = this.formatTimeTextboxes();
		var beforeOrAfterArr = this.formatBeforeOrAfterSelections();
		var booleansArr = this.formatBooleanSelections();
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
	},
	makeResetButton: function() {
		$('#queryFactory').append("<button id='resetBtn' type='button' onclick='weatherspot_queryFactory.resetButtonOnClick()'>Reset</button>");
	},
	resetButtonOnClick: function() {
		$('.beforeOrAfter').remove();
		$('.datePicker').remove();
		$('.timePicker').remove();
		$('.booleanChoice').remove();	
		while (this.WHERECount > 0) {
			$('#queryFactory br:nth-last-of-type(3)').remove();
			--this.WHERECount;
		}
		$('#resetBtn').remove();
	},


	WHEREClauseButtonOnClick: function() {
		$('#queryBtn').remove();
		$('#resetBtn').remove();
		$('#queryFactory br:last-of-type').remove();
		$('#queryFactory br:last-of-type').remove();
		$('#queryFactory p:last-of-type').remove();
		$('#limitTextbox').remove();
		if ($('#queryFactory br').length > 0) {
			this.makeBooleanChoiceSelection();
		}
		++this.WHERECount;

		$('#queryFactory').append('<br>');
		this.makeBeforeOrAfterSelection();
		this.makeDateEntryTextbox();
		this.makeTimeEntryTextbox();
		this.makeWHEREClauseButton();
		$('#queryFactory').append('<br>');
		this.makeLimitTextbox();
		$('#queryFactory').append('<br>');
		this.makeQueryButton();
		this.makeResetButton();
		
	},
	makeQueryButton: function() {
		$('#queryFactory').append("<button id='queryBtn' type='button' onclick='weatherspot_queryFactory.queryCallback(weatherspot_queryFactory.formToQueryString())'>" + this.buttonText + "</button>");
	},

	makeWHEREClauseButton: function() {
		var appendStr = "<a href='javascript:void(0);' style='display: inline' onclick='weatherspot_queryFactory.WHEREClauseButtonOnClick(); $(this).remove()'>Add Where Clause</a>"
		$('#queryFactory').append(appendStr);
	},
	makeLabel: function() {
		var prependStr = "<p>All Times are UTC</p>";
		$('#factoryContainer').prepend(prependStr);
	},
	makeLimitTextbox: function() {
		var appendStr = "<p style='display:inline; padding-left: 4px'>Query Limit:<input value='1' type='number' id='limitTextbox' style='width: 6em'></input></p>";
		$('#queryFactory').append(appendStr);
		
	},
	makeQueryFactory: function(appendToElem, _buttonText, _queryCallback) {
		this.buttonText = _buttonText;
		this.queryCallback = _queryCallback;

		$(appendToElem).append("<div id='factoryContainer' style='outline: pink solid 1px'><div id='queryFactory'></div></div>");
		this.makeSeriesSelection();
		this.makeMeasurementSelection();
		this.makeWHEREClauseButton();
		$('#queryFactory').append('<br>');
		this.makeLimitTextbox();
		$('#queryFactory').append('<br>');
		this.makeQueryButton();
		this.makeLabel();
	}
};
