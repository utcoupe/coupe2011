
// Logs
bigHtml = '';
for (var i=0; i<50; i++) bigHtml += (new Date)+': '+i+' go position '+(Math.random()*10)+', '+(Math.random()*10)+'<br>';
var panelLogs = new Ext.Panel({
	cls: 'lalalala',
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

// API
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
	recalage: function(){
		$.get('/api/recalage/1', function(data){
			Ext.Msg.alert('Requête recalage', "Réponse: "+data);
		});
	}
};

// Controls
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
	cls: 'lalalala',
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
                    text: 'Recalage',
                    handler: api.recalage
                },
                {xtype: 'spacer'},
                
                {
                    text: 'Stop',
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
                {iconCls: 'arrow_left', handler: function(){ updateRobot(-20, 0); }},
                {iconCls: 'arrow_up', handler: function(){ updateRobot(0, -20); }},
                {iconCls: 'arrow_down', handler: function(){ updateRobot(0, 20); }},
                {iconCls: 'arrow_right', handler: function(){ updateRobot(20, 0); }},
                {iconCls: 'refresh', handler: function(){ updateRobot(0, 0, -90); }}
            ]
        }
    ]
});


// Structure
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