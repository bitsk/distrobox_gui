import QtQuick 2.15
import QtQuick.Controls 2.15 as T
import org.ukui.quick.items 1.0 as UkuiItems
import org.ukui.quick.platform 1.0

T.TextField {
    id: control
    
    background: UkuiItems.DtThemeBackground {
        radius: 6
        backgroundColor: control.activeFocus ? GlobalTheme.kComponentInput :
                         GlobalTheme.kComponentNormal
        border.width: control.activeFocus ? 2 : 1
        borderColor: control.activeFocus ? GlobalTheme.highlightActive : GlobalTheme.kLineNormal
    }
    
    color: GlobalTheme.kFontPrimary.pureColor
    selectionColor: GlobalTheme.highlightActive.pureColor
    placeholderTextColor: GlobalTheme.kFontPlaceholderText.pureColor
}
