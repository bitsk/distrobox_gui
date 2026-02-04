import QtQuick 2.15
import QtQuick.Controls 2.15 as T
import org.ukui.quick.items 1.0 as UkuiItems
import org.ukui.quick.platform 1.0

T.Button {
    id: control
    property color customTextColor: "transparent"
    
    background: UkuiItems.DtThemeBackground {
        radius: 6
        backgroundColor: {
             if (control.pressed) return control.highlighted ? GlobalTheme.kBrandClick : GlobalTheme.kComponentClick
             if (control.hovered) return control.highlighted ? GlobalTheme.kBrandHover : GlobalTheme.kComponentHover
             return control.highlighted ? GlobalTheme.kBrandNormal : GlobalTheme.kComponentNormal
        }
        border.width: control.visualFocus ? 2 : 0
        borderColor: GlobalTheme.highlightActive
    }

    contentItem: UkuiItems.DtThemeText {
        text: control.text
        
        // Determine the default theme color based on state
        property var defaultThemeColor: control.highlighted ? GlobalTheme.kWhite : (control.enabled ? GlobalTheme.kFontPrimary : GlobalTheme.kFontPrimaryDisable)
        
        // Assign to textColor to satisfy DtThemeText requirement (though we override color)
        textColor: defaultThemeColor
        
        // Override color property logic: use custom if set, otherwise use pureColor from theme
        color: control.customTextColor.a !== 0 ? control.customTextColor : defaultThemeColor.pureColor
        
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
