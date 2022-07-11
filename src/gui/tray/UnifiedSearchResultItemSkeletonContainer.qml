import QtQml 2.15
import QtQuick 2.15
import Style 1.0

Column {
    id: unifiedSearchResultsListViewSkeletonColumn

    Repeater {
        model: 10
        UnifiedSearchResultItemSkeleton {
            width: unifiedSearchResultsListViewSkeletonColumn.width
        }
    }
}
