/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



package ifc.text;

import lib.MultiPropertyTest;

import com.sun.star.container.XNameContainer;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.xml.AttributeData;

/**
* Testing <code>com.sun.star.text.CellProperties</code>
* service properties :
* <ul>
*  <li><code> BackColor</code></li>
*  <li><code> BackGraphicURL</code></li>
*  <li><code> BackGraphicFilter</code></li>
*  <li><code> BackGraphicLocation</code></li>
*  <li><code> NumberFormat</code></li>
*  <li><code> BackTransparent</code></li>
*  <li><code> LeftBorder</code></li>
*  <li><code> RightBorder</code></li>
*  <li><code> TopBorder</code></li>
*  <li><code> BottomBorder</code></li>
*  <li><code> LeftBorderDistance</code></li>
*  <li><code> RightBorderDistance</code></li>
*  <li><code> TopBorderDistance</code></li>
*  <li><code> BottomBorderDistance</code></li>
*  <li><code> UserDefinedAttributes</code></li>
*  <li><code> TextSection</code></li>
*  <li><code> IsProtected</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'CellProperties.TextSection'</code> (of type
*  <code>XInterface</code>):
*   instance of <code>com.sun.star.text.TextSection</code></li>
* </ul> <p>
* The following predefined files needed to complete the test:
* <ul>
*  <li> <code> crazy-blue.jpg </code> : jpeg image used to test
*  BackGraphicURL()</li>
*  <li> <code> space-metal.jpg </code> : jpeg image used to test
*  BackGraphicURL()</li>
* </ul> <p>
* Properties testing is automated by <code>lib.MultiPropertyTest</code>.
* @see com.sun.star.text.CellProperties
*/
public class _CellProperties extends MultiPropertyTest {

    /**
    * This property can be void, so new value must be specified from
    * corresponding object relation.
    */
    public void _TextSection() {
        log.println("Testing with custom Property tester") ;
        testProperty("TextSection", new PropertyTester() {
            protected Object getNewValue(String propName, Object oldValue) {
                return tEnv.getObjRelation("CellProperties.TextSection");
            }
        });
    }


    /**
    * Redefined method returns value, that differs from property value.
    */
    public void _BackColor() {
        final Short val1 = new Short( (short) 4 );
        final Short val2 = new Short( (short) 6 );
        log.println("Testing with custom Property tester") ;
        testProperty("BackColor", new PropertyTester() {
            protected Object getNewValue(String propName, Object oldValue) {
                if ( oldValue.equals(val1) )
                    return val2;
                else
                    return val1;
            }
        });
    }


    /**
    * Redefined method returns value, that differs from property value.
    */
    public void _BackGraphicURL() {
        log.println("Testing with custom Property tester") ;
        testProperty("BackGraphicURL", new PropertyTester() {
            protected Object getNewValue(String propName, Object oldValue) {
                if (oldValue.equals(util.utils.getFullTestURL
                        ("space-metal.jpg")))
                    return util.utils.getFullTestURL("crazy-blue.jpg");
                else
                    return util.utils.getFullTestURL("space-metal.jpg");
            }
        });
    }


    /**
    * Redefined method returns object, that contains changed property value.
    */
    public void _UserDefinedAttributes() {
        log.println("Testing with custom property tester");
        testProperty("UserDefinedAttributes", new PropertyTester() {
            protected Object getNewValue(String propName, Object oldValue) {
                XNameContainer NC = null;
                try {
                    NC = (XNameContainer) 
                    AnyConverter.toObject(new Type(XNameContainer.class),oldValue);
                    NC.insertByName("MyAttribute",
                        new AttributeData("","CDATA","Value"));
                } catch ( com.sun.star.lang.IllegalArgumentException e ) {
                    log.println("Failed to check 'UserDefinedAttributes'");
                    e.printStackTrace(log);
                } catch ( com.sun.star.lang.WrappedTargetException e ) {
                    log.println("Failed to check 'UserDefinedAttributes'");
                    e.printStackTrace(log);
                } catch ( com.sun.star.container.ElementExistException e ) {
                    log.println("Failed to check 'UserDefinedAttributes'");
                    e.printStackTrace(log);
                }
                return NC;
            }
        });
    }

} //finish class _CellProperties
