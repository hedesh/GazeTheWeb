window.domLinks = [];
window.domTextInputs = [];

/* TODOs
        - Write method 'InformCEF' -- DONE
        - Delete methods with equal name in old approach
            > UpdateDOMRects()
            > ...
        - Adjust RenderProcessHandler to new approach
*/

/**
 * Create a DOMObject of given type for node and add it to the global list
 * Also, automatically inform CEF about added node and in case of Rect updates
 * 
 * args:    node : DOMNode, nodeType : int
 * returns: void
 */
function CreateDOMObject(node, nodeType)
{
    // Create DOMObject, which encapsulates the given DOM node
	var domObject = {
    /* Attributes */
		_node: node,
		_nodeType: nodeType,
		_rects: node.getClientRects(),
		_visible: true,

    /* Methods */ 
		// Update member variable for Rects and return if an update has occured 
		updateRects: function(){
			var fetchRects = this._node.getClientRects();
			var equal = CompareClientRects(fetchRects, this._rects);

			if(!equal)
            {
                this._rects = rects;
                InformCEF(this, ['update', 'rects']); 
            }
			
			return !equal;
		},

		// Returns float[4] for each Rect with adjusted coordinates
		getRects: function(){
			// Update _rects if changes occured
			this.updateRects();
			// Return _rects as list of float lists with adjusted coordinates
			return AdjustClientRects(this._rects);
		},
	};

    // Push to list and determined by DOMObjects position in type specific list
    var domObjList = GetDOMObjectList(nodeType);
    domObjList.push(domObj);
    var nodeID = domObjList.length - 1;

    // Add attributes to given DOM node
    node.setAttribute('nodeID', nodeID);
    node.setAttribute('nodeType', nodeType);

    InformCEF(this, ['added']);
}

function CreateDOMTextInput(node) { CreateDOMObject(node, 0); }
function CreateDOMLink(node) { CreateDOMLink(node, 1); }


/**
 * Adjusts given DOMRects to window properties in order to get screen coordinates
 * 
 * args:    rects : [DOMRect]
 * return:  [float] - top, left, bottom, right coordinates of each DOMRect in rects
 */
function AdjustClientRects(rects)
{
	function RectToFloatList(rect){ return [rect.top, rect.left, rect.bottom, rect.right]; };

	var lists = [];
	for(var i = 0, n = rects.length; i < n; i++)
	{
		lists.push(RectToFloatList(rects[i]));
	}

	// TODO!: Adjust coordinates to window settings

	return lists;
}

/**
 * Compares two lists of DOMRect objects and returns true if all values are equal
 * 
 * args:    rects1, rects2 : [DOMRect]
 * returns: bool
 */
function CompareClientRects(rects1, rects2)
{
	var n = rects1.length;

	if(n != rects2.length)
		return false;

	// Check if width and height of each Rect are identical
	for(var i = 0; i < n; i++)
	{
		if(rects1[i].width != rects2[i].width || rects1[i].height != rects2[i].height)
			return false;
	}

	// Check if Rect coordinates are identical
	for(var i = 0; i < n; i++)
	{
		// Note: It's enough to check x & y if width & height are identical
		if(rects1[i].x != rects2[i].x || rects1[i].y != rects2[i].y)		
			return false;
	}

	return true;
}

/**
 * Triggers update of DOMRects of each DOMObject by using DOMObjects updateRects() method
 * 
 * args:    -/-
 * returns: void
 */
function UpdateDOMRects()
{
    // Trigger update of Rects for every domObject
    window.domTextInputs.forEach(
        function (domObj) { domObj.updateRects(); }
    );
    window.domLinks.forEach(
        function (domObj) { domObj.updateRects(); }
    );
}

/**
 * Transform natural language to encoded command to send to CEF
 * 
 * args:    domObj : DOMObject, operation : [string]
 * returns: void
 */
function InformCEF(domObj, operation)
{
    var id = domObj._node.getAttribute('noteID');
    var type = domObj._nodeType;

    // Encoding uses only first 3 chars of natural language operation
    var op = operation[0].substring(0,3);

    var encodedCommand = 'DOM#'+op+'#'+type+'#'+id+'#';

    if(op == 'upd')
    {
        if(operation[1] == 'rect')
        {
            var rectsData = domObj.getRects();
            // Encode changes in 'rect' as attribute '0'
            encodedCommand += '1#';
            // Encode list of floats to strings, each value separated by ';'
            for(var i = 0, n = rectsData.length; i < n; i++)
            {
                encodedCommand += (rectsData[i]+';');
            }
            // Add '#' at the end to mark ending of encoded command
            encodedCommand += '#';
        }

        if(operation[i] == 'fixed')
        {
            // If fixed attribute doesn't exist, node is not fixed
            var status = (domObj._node.getAttribute('fixedID')) ? 1 : 0;
            // Encode changes in 'fixed' as attribute '1'
            encodedCommand += ('1#'+status+'#');
        }
    }

    // Send encoded command to CEF
    ConsolePrint(encodedCommand);
}

/**
 * Get global list of DOMObjects for specific node 
 * 
 * args:    nodeType : int
 * returns: [DOMObject]
 */
function GetDOMObjectList(nodeType)
{
    switch(nodeType)
    {
        case 0: { return window.domTextInputs; };
        case 1: { return window.domLinks; };
        // NOTE: Add more cases if new nodeTypes are added
        default:
        {
            ConsolePrint('ERROR: No DOMObjectList for nodeType='+nodeType+' exists!');
            return undefined;
        }
    }
}

/**
 * Get DOMObject by using node's type and ID
 * 
 * args:    nodeType, nodeID : int
 * returns: DOMObject
 */
function GetDOMObject(nodeType, nodeID)
{
    var targetList = GetDOMObjectList(nodeType);

    // Catch error case
    if(nodeID >= targetList.length || targetList == undefined)
    {
        ConsolePrint('ERROR: Node with id='+nodeID+' does not exist for type='+nodeType+'!');
        return undefined;
    }

    return targetList[nodeID];
}

/**
 * Get corresponding DOMObject to a given node, if it doesn't exist 'undefined' is returned
 * 
 * args:    node : DOMNode
 * return:  DOMObject
 */
function GetDOMObject(node)
{
    var id = node.getAttribute('nodeID');
    var type = node.getAttribute('nodeType');

    if(!id || !type)
        return undefined;

    return GetDOMObject(type, id);
}