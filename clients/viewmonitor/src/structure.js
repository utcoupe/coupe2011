// -------------------------------
// Logs
// -------------------------------

bigHtml = '';
for (var i=0; i<50; i++) bigHtml += (new Date)+': '+i+' go position '+(Math.random()*10)+', '+(Math.random()*10)+'<br>';
var panelLogs = new Ext.Panel({
	styleHtmlContent: true,
	html: '<div id="logs">'+bigHtml+'</div>',
	scroll: 'vertical',
    dockedItems: [
        {
            dock: 'top',
            xtype: 'toolbar',
            items: [
                {
                    text: 'Clear',
                    handler: function() {
                        $('#logs').empty();
                    }
                }
            ]
        }
    ]
});


// -------------------------------
// API
// -------------------------------

$(function(){
	$.ajaxSetup({
		error: function(jqXHR, textStatus, errorThrown){
			Ext.Msg.alert('Erreur liaison', "Le serveur web a retourné: "+errorThrown);
		}
	});
});
var api = {
	ping: function(){
		$.get('/api/ping/1', function(data){
			Ext.Msg.alert('Requête ping', "Réponse: "+data);
		});
	},
	recalageRouge: function(){
		$.get('/api/recalageRouge/1', function(data){
			//Ext.Msg.alert('Requête recalage rouge', "Réponse: "+data);
		});
	},
	recalageBleu: function(){
		$.get('/api/recalageBleu/1', function(data){
			//Ext.Msg.alert('Requête recalage bleu', "Réponse: "+data);
		});
	},
	stop: function(){
		$.get('/api/stop/1', function(data){
			//Ext.Msg.alert('Requête stop', "Réponse: "+data);
		});
	},
	left: function(){
		var x=-100, y=0;
		$.get('/api/gorelative/'+x+'/'+y);
	},
	right: function(){
		var x=100, y=0;
		$.get('/api/gorelative/'+x+'/'+y);
	},
	down: function(){
		var x=0, y=-100;
		$.get('/api/gorelative/'+x+'/'+y);
	},
	up: function(){
		var x=0, y=100;
		$.get('/api/gorelative/'+x+'/'+y);
	}
};

      
// -------------------------------          
// Controls
// -------------------------------

$(function(){
	$('#table').empty();
});
var rx = ry = ra = 0;
function updateRobot(x, y, a)
{
	rx += x;
	ry += y;
	ra += a || 0;
	$('.robot').animate({'left':rx+'px', 'top': ry+'px', 'rotate': ra+'deg'});
}
var panelControls = new Ext.Panel({
	styleHtmlContent: true,
	html: '<div id="controls"><center><div id="table"><img class="table" src="resources/img/tabletmp.png" /><img class="robot" src="resources/img/robotmini.png" /></div></center></div>',
	scroll: 'vertical',
    dockedItems: [
        {
            dock: 'top',
            xtype: 'toolbar',
            items: [
                {
                    text: 'Ping',
                    handler: api.ping
                },
                {
                    text: 'InitRouge',
                    handler: api.recalageRouge
                },
                {
                    text: 'InitBleu',
                    handler: api.recalageBleu
                },
                {xtype: 'spacer'},
                
                {
                    text: 'Stop',
                    handler: api.stop,
                    ui: 'decline'
                },
                {
                    text: 'Pause'
                },
                {
                    text: 'Resume'
                }
            ]
        },
        {
            dock: 'bottom',
            xtype: 'toolbar',
		    layout: {
		        pack: 'center'
		    },
		    defaults: {
		        iconMask: true,
		        ui: 'plain'
		    },
            items: [
                {iconCls: 'refresh', handler: function(){ updateRobot(0, 0, +90); }},
                {iconCls: 'arrow_left', handler: function(){ updateRobot(-20, 0); api.left(); }},
                {iconCls: 'arrow_up', handler: function(){ updateRobot(0, -20); api.up(); }},
                {iconCls: 'arrow_down', handler: function(){ updateRobot(0, 20); api.down(); }},
                {iconCls: 'arrow_right', handler: function(){ updateRobot(20, 0); api.right(); }},
                {iconCls: 'refresh', handler: function(){ updateRobot(0, 0, -90); }}
            ]
        }
    ]
});

// -------------------------------
// Visio
// -------------------------------

var cam1 = new Ext.Panel({
	scroll: 'vertical',
	title: 'Webcam 1',
	items: [
	{
        title: 'Sliders',
        xtype: 'form',
        items: [{
		    xtype: 'fieldset',
		    title: 'Aperçu',
		    items: {
				styleHtmlContent: true,
				html: '<center><img class="radius" src="resources/img/cam_test.jpg" height="150" />  <img class="radius" src="resources/img/cam_test_handled.jpg" height="150" />  <img src="resources/img/cam_test_handled_traced.jpg" class="radius" height="150" /></center>'
			}
		}, {
            xtype: 'fieldset',
            title: 'Configuration',
            items: [{
                xtype: 'selectfield',
                name: 'options',
                label: 'Couleur',
                options: [
                    {text: 'jaune (1)',  value: '1'},
                    {text: 'blanc (2)', value: '2'},
                    {text: 'noir (3)', value: '3'}
                ]
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune H',
                minValue: 0,
				maxValue: 255,
				value: 0
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune V',
                minValue: 0,
				maxValue: 255,
				value: 50
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune tolérance H',
                minValue: 0,
				maxValue: 255,
				value: 100
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune tolérance V',
                minValue: 0,
				maxValue: 255,
				value: 255
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc V'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc tolérance H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc tolérance V'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir V'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir tolérance H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir tolérance V'
            }]
        }]
    }]
});
var cam2 = new Ext.Panel({
	scroll: 'vertical',
	title: 'Webcam 2',
	items: [
	{
        title: 'Sliders',
        xtype: 'form',
        items: [{
		    xtype: 'fieldset',
		    title: 'Aperçu',
		    items: {
				styleHtmlContent: true,
				html: '<center><img class="radius" src="resources/img/cam_test.jpg" height="150" />  <img class="radius" src="resources/img/cam_test_handled.jpg" height="150" />  <img src="resources/img/cam_test_handled_traced.jpg" class="radius" height="150" /></center>'
			}
		}, {
            xtype: 'fieldset',
            title: 'Configuration',
            items: [{
                xtype: 'selectfield',
                name: 'options',
                label: 'Couleur',
                options: [
                    {text: 'jaune (1)',  value: '1'},
                    {text: 'blanc (2)', value: '2'},
                    {text: 'noir (3)', value: '3'}
                ]
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune H',
                minValue: 0,
				maxValue: 255,
				value: 0
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune V',
                minValue: 0,
				maxValue: 255,
				value: 50
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune tolérance H',
                minValue: 0,
				maxValue: 255,
				value: 100
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Jaune tolérance V',
                minValue: 0,
				maxValue: 255,
				value: 255
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc V'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc tolérance H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Blanc tolérance V'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir V'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir tolérance H'
            }, {
                xtype: 'sliderfield',
                name: 'value',
                label: 'Noir tolérance V'
            }]
        }]
    }]
});
var panelVisio = new Ext.TabPanel ({items: [cam1, cam2],
    dockedItems: [
        {
            dock: 'bottom',
            xtype: 'toolbar',
            items: [
                {
                    text: 'Sauver la config',
                    ui: 'confirm'
                }
            ]
        }
	]
});

// -------------------------------
// Balises
// -------------------------------

var fields = [
	{
		xtype: 'fieldset',
		title: 'Balise 1',
		items: [
			{xtype: 'textfield', label: 'IP', value: '192.168.1.31'},
			{xtype: 'checkboxfield', label: '<img class="radius" src="resources/img/cam_test.jpg" height="100" />', checked: true, name: 'a'}
		]
	},{
		xtype: 'fieldset',
		title: 'Balise 2',
		items: [
			{xtype: 'textfield', label: 'IP', value: '192.168.1.32'},
			{xtype: 'checkboxfield', label: '<img class="radius" src="resources/img/cam_test.jpg" height="100" />', checked: true, name: 'a'}
		]
	},{
		xtype: 'fieldset',
		title: 'Balise 3',
		items: [
			{xtype: 'textfield', label: 'IP', value: '192.168.1.33'},
			{xtype: 'checkboxfield', label: 'Pas d\'image', checked: false, name: 'a'}
		]
	}
];

var panelBalises = new Ext.Panel({
    dockedItems: [
        {
            dock: 'bottom',
            xtype: 'toolbar',
            items: [
                {
                    text: 'Tester ping + image'
                },
                {
                    text: 'Sauver la config',
                    ui: 'confirm'
                }
            ]
        }
	],
	scroll: 'vertical',
    items:
    {
    	xtype: 'form',
    	items: fields
    }
});

/*
 [{
    	xtype: 'panel',
    	items: [{
				width: '33%',
				items: {
					xtype: 'fieldset',
					items: [
						{xtype: 'textfield', label: 'IP'},
						{xtype: 'togglefield', label: 'Activé'}
					]
				}
			},{
				width: '33%',
				html: 'Second'
			},{
				width: '33%',
				html: 'Third'
			}
		]
    }]
*/

// -------------------------------
// Structure
// -------------------------------

sink.Structure = [
	{
		text: 'Logs',
		card: panelLogs,
		preventHide: true,
		cardSwitchAnimation: {
		    type: 'slide'
		},
		leaf: true
	},
	{
		text: 'Controls',
		card: panelControls,
		preventHide: true,
		cardSwitchAnimation: {
		    type: 'slide'
		},
		leaf: true
	},
	{
		text: 'Visio',
		card: panelVisio,
		preventHide: true,
		cardSwitchAnimation: {
		    type: 'slide'
		},
		leaf: true
	},
	{
		text: 'Balises',
		card: panelBalises,
		preventHide: true,
		cardSwitchAnimation: {
		    type: 'slide'
		},
		leaf: true
	}
];

Ext.regModel('Demo', {
    fields: [
        {name: 'text',        type: 'string'},
        {name: 'source',      type: 'string'},
        {name: 'preventHide', type: 'boolean'},
        {name: 'cardSwitchAnimation'},
        {name: 'card'}
    ]
});

sink.StructureStore = new Ext.data.TreeStore({
    model: 'Demo',
    root: {
        items: sink.Structure
    },
    proxy: {
        type: 'ajax',
        reader: {
            type: 'tree',
            root: 'items'
        }
    }
});
