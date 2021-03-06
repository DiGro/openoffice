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



#ifndef __FRAMEWORK_UIELEMENT_OBJECTMENUCONTROLLER_HXX_
#define __FRAMEWORK_UIELEMENT_OBJECTMENUCONTROLLER_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <macros/xserviceinfo.hxx>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XPopupMenuController.hpp>
#include <com/sun/star/embed/VerbDescriptor.hpp>

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________
#include <svtools/popupmenucontrollerbase.hxx>
#include <toolkit/awt/vclxmenu.hxx>
#include <cppuhelper/weak.hxx>
#include <rtl/ustring.hxx>

namespace framework
{
	class ObjectMenuController :  public svt::PopupMenuControllerBase
    {
		using svt::PopupMenuControllerBase::disposing;

		public:
		    ObjectMenuController( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
		    virtual ~ObjectMenuController();

            // XServiceInfo
		    DECLARE_XSERVICEINFO

		    // XStatusListener
		    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );

		    // XEventListener
		    virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& Source ) throw ( ::com::sun::star::uno::RuntimeException );

		private:
            void fillPopupMenu( const com::sun::star::uno::Sequence< com::sun::star::embed::VerbDescriptor >& rVerbCommandSeq, com::sun::star::uno::Reference< com::sun::star::awt::XPopupMenu >& rPopupMenu );
            virtual void impl_select(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >& _xDispatch,const ::com::sun::star::util::URL& aURL);

            com::sun::star::uno::Reference< com::sun::star::frame::XDispatch >  m_xObjectUpdateDispatch;
    };
}

#endif // __FRAMEWORK_UIELEMENT_OBJECTMENUCONTROLLER_HXX_
